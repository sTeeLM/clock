#include "sm_fuse_timer.h"
#include "debug.h"
#include "mod_common.h"
#include "led.h"
#include "lt_timer.h"
#include "rom.h"
#include "thermo.h"
#include "mpu.h"
#include "hg.h"
#include "fuse.h"
#include "power.h"

#define next_arm_step       last_display_s
#define in_rollback         lpress_lock_year_hour
#define password_index      lpress_start   // current password index
#define password_content    common_state   // current displayed password
#define verify_state        last_display_s // verify state， last 6 bits is 0x3F is success， 2 bits retry count

const char * code sm_fuse_timer_ss_name[] = 
{
  "SM_FUSE_TIMER_INIT",
  "SM_FUSE_TIMER_PREARMED",
  "SM_FUSE_TIMER_ARMED",
  "SM_FUSE_TIMER_VERIFY",
  "SM_FUSE_TIMER_DISARMED",
  "SM_FUSE_TIMER_PREDETONATE",
  NULL
};

enum timer_arm_step {
  TIMER_ARM_FUSE = 0, 
  TIMER_ARM_MPU,
  TIMER_ARM_HG,
  TIMER_ARM_THERMO,
  TIMER_ARM_LT_TIMER,
  TIMER_ARM_DELAY0,
  TIMER_ARM_DELAY1,
  TIMER_ARM_DELAY2,
  TIMER_ARM_DELAY3,
  TIMER_ARM_DELAY4,
  TIMER_ARM_STEP_CNT
};

enum timer_arm_err {
	TIMER_ERR_OK = 0,
	TIMER_ERR_FUSE0_BROKE,
	TIMER_ERR_FUSE1_BROKE,
	TIMER_ERR_TRIPWIRE_BROKE,
	TIMER_ERR_THERMO_HI_HIT,
	TIMER_ERR_THERMO_LO_HIT,
	TIMER_ERR_THERMO_LO_GE_HI, // lo threshold >= hi threshold
	TIMER_ERR_HG_HIT,
	TIMER_ERR_MPU_HIT,
	TIMER_ERR_LT_TIMER_HIT,
	TIMER_ERR_LT_TIMER_TOO_CLOSE,
	
};

enum timer_display_state
{
  DISPLAY_TIMER_DISARMED,
  DISPLAY_TIMER_PREDETONATE,
};


static unsigned char check_and_set(unsigned char step)
{
	unsigned char ret = TIMER_ERR_OK;
	char val, val1;
	switch(step) {
		case TIMER_ARM_FUSE: 
			fuse_enable(1);
			break;
		case TIMER_ARM_MPU:
			val = rom_read(ROM_FUSE_MPU);
			mpu_enable(val != MPU_THRESHOLED_INVALID);
			break;
		case TIMER_ARM_HG:
			val = rom_read(ROM_FUSE_HG_ONOFF);
			mpu_enable(val == 1);
			break;
		case TIMER_ARM_THERMO:
			val = rom_read(ROM_FUSE_THERMO_HI);
			val1 = rom_read(ROM_FUSE_THERMO_LO);
			if(val != THERMO_THRESHOLED_INVALID || val1 != THERMO_THRESHOLED_INVALID) {
				if(val != THERMO_THRESHOLED_INVALID && val1 != THERMO_THRESHOLED_INVALID) {
					if(val <= val1) {
						ret = TIMER_ERR_THERMO_LO_GE_HI;
						break;
					}
				}
				thermo_enable(1);
			}
			break;
		case TIMER_ARM_LT_TIMER:
     lt_timer_sync_from_rom();
      lt_timer_sync_to_rtc();
      if(!lt_timer_get_relative(1)) {
        ret = TIMER_ERR_LT_TIMER_TOO_CLOSE;
        break;
      } else {
        lt_timer_start_ram();
      }
      break;
		case TIMER_ARM_DELAY0:
		case TIMER_ARM_DELAY1:
		case TIMER_ARM_DELAY2:
		case TIMER_ARM_DELAY3:
		case TIMER_ARM_DELAY4:	
			break;
	}
	return ret;
}

static void roll_back(bit include_fuse)
{
  thermo_enable(0);
  mpu_enable(0);
  hg_enable(0);
  lt_timer_reset();
	if(include_fuse) {
		fuse_enable(0);
	}
}

static void display_prearm(unsigned char step, unsigned char err)
{
  led_clear();
  led_set_code(5, 'P');
  led_set_code(4, 'R');
  led_set_code(3, '-');
	led_set_code(2, step + 0x30);
  led_set_code(1, '-');
  led_set_code(0, err + 0x30);
}

static void display_timer(unsigned char what)
{
  led_clear();
  led_set_code(5, 'P');
  led_set_code(4, 'R');
  led_set_code(3, '-');
  switch(what) {
    case DISPLAY_TIMER_DISARMED:
			led_set_code(2, 'D');
      led_set_code(1, 'D');
      led_set_code(0, 'D');
      break;
    case DISPLAY_TIMER_PREDETONATE:
			led_set_code(2, 'C');
      led_set_code(1, 'C');
      led_set_code(0, 'C');
      break;
  }
}

static void verify_password()
{
  unsigned char res;
  
  res = rom_read(ROM_FUSE_PASSWORD + password_index);
  
  verify_state &= ~(1 << password_index);
  
  if(res == password_content) {
    verify_state |= (1 << password_index);
    CDBG("verify_password %bd : %bd <->%bd success\n", password_index, res, password_content);
  } else {
    CDBG("verify_password %bd : %bd <->%bd failed\n", password_index, res, password_content);
  }
  return;
}

static void display_password()
{
  unsigned char i;
  led_clear();
  for( i = 0 ; i < 6 ; i ++ ) {
    if(i < password_index) {
      led_set_code(i, '-');
    } else if(i == password_index) {
      led_set_code(i, password_content + 0x30);
    } else {
      led_set_code(i, 'O');
    }
  }
}

static void inc_password(void)
{
  password_content = (++ password_content) % 10;
}

void sm_fuse_timer_init(unsigned char from, unsigned char to, enum task_events ev)
{
  CDBG("sm_fuse_timer_init %bd %bd %bd\n", from, to, ev);
	display_logo(DISPLAY_LOGO_TYPE_FUSE, 3);
	next_arm_step = 0;
	in_rollback   = 1;
}

// pre-arm
void sm_fuse_timer_submod0(unsigned char from, unsigned char to, enum task_events ev)
{
	unsigned char err;
  CDBG("sm_fuse_timer_submod0 %bd %bd %bd\n", from, to, ev);
	
	if(ev == EV_KEY_SET_UP) {
		err = TIMER_ERR_OK;
		next_arm_step = 0;
		in_rollback   = 0;
		display_prearm(next_arm_step, err);
		return;
	}
	
	if(ev == EV_1S && in_rollback == 0) {
		if(next_arm_step < TIMER_ARM_STEP_CNT) {
			err = check_and_set(next_arm_step); // TIMER_ERR_THERMO_LO_GE_HI/TIMER_ERR_LT_TIMER_TOO_CLOSE
			display_prearm(next_arm_step, err);
			if(err != TIMER_ERR_OK) {
				in_rollback = 1;
				roll_back(1);
				set_task(EV_KEY_V0); // 设置传感器出错，进入sm_fuse_test
				return;
			}
			next_arm_step ++;
		} else {
			set_task(EV_KEY_V1); // 进入armed状态
		}
		return;
	}
	
	if((ev == EV_COUNTER || ev == EV_FUSE0_BROKE || ev == EV_FUSE1_BROKE
			|| ev == EV_MOT_MPU || ev == EV_ROTATE_HG || ev == EV_FUSE_TRIPWIRE
			|| ev == EV_THERMO_HI || ev == EV_THERMO_LO ) && in_rollback == 0) {
			switch(ev)
			{
				case EV_COUNTER:     err = TIMER_ERR_LT_TIMER_HIT; break;
				case EV_FUSE0_BROKE: err = TIMER_ERR_FUSE0_BROKE; break;
				case EV_FUSE1_BROKE: err = TIMER_ERR_FUSE1_BROKE; break;
				case EV_MOT_MPU:     err = TIMER_ERR_MPU_HIT; break;
				case EV_ROTATE_HG:   err = TIMER_ERR_HG_HIT; break;
				case EV_FUSE_TRIPWIRE: err = TIMER_ERR_TRIPWIRE_BROKE; break;
				case EV_THERMO_HI:   err = TIMER_ERR_THERMO_HI_HIT; break;
				case EV_THERMO_LO:   err = TIMER_ERR_THERMO_LO_HIT; break;
			}
			display_prearm(next_arm_step, err);
			in_rollback = 1;
			roll_back(1);
			set_task(EV_KEY_V0); // 设置传感器出错，进入sm_fuse_test
		return;
	}
}

// armed
void sm_fuse_timer_submod1(unsigned char from, unsigned char to, enum task_events ev)
{
  CDBG("sm_fuse_timer_submod1 %bd %bd %bd\n", from, to, ev);
	
	if(ev == EV_KEY_V1 || ev == EV_KEY_SET_LPRESS) {
		lt_timer_display(1);
		// 如果从PREARMED进入，清除密码重试次数
		if(get_sm_ss_state(from) == SM_FUSE_TIMER_PREARMED) {
			verify_state   = 0;
		}
    power_reset_powersave_to();
		return;
	}
	
	if(ev == EV_1S) {
		power_test_powersave_to();
		return;
	}
	
	if(ev == EV_KEY_SET_PRESS) {
		power_reset_powersave_to();
		lt_timer_switch_display();
		return;
	}
	
	if(ev == EV_KEY_MOD_PRESS) {
		power_reset_powersave_to();
		return;
	}
}

// verify
void sm_fuse_timer_submod2(unsigned char from, unsigned char to, enum task_events ev)
{
  CDBG("sm_fuse_timer_submod2 %bd %bd %bd\n", from, to, ev);
	if(ev == EV_KEY_MOD_PRESS) {
		// 进入密码验证状态
		if(get_sm_ss_state(from) == SM_FUSE_TIMER_ARMED) {
			password_index   = 5;
			password_content = 0;
			verify_state   &= 0xC0;  // 保留最高2bit，密码重试次数
			lt_timer_display(0);
			display_password();
		} else if(get_sm_ss_state(from) == SM_FUSE_TIMER_VERIFY) {
			// 验证下一位密码
			if(password_index > 0) {
				verify_password();
				password_content = 0;
				password_index --;
				display_password(); 
			} else {
				// 校验结果，如果通过，EV_KEY_V0
				// 如果失败小于3次，EV_KEY_V1
				// 如果失败大于等于3次，EV_KEY_V2
				verify_password();
				if((verify_state & 0x3F) == 0x3F) {
					CDBG("verify pass!\n");
					set_task(EV_KEY_V0); // 校验通过了
				} else if((verify_state & 0xC0) == 0x80) {
					CDBG("verify failed, to detonate!\n");
					set_task(EV_KEY_V2); // 校验多次不OK，挂了
				} else {
					verify_state += 0x40;
					CDBG("verify failed, count is %0bx!\n", (verify_state & 0xC0));
					set_task(EV_KEY_V1);
				}
			}
		}
		return;
	}
	
	if(ev == EV_KEY_SET_PRESS) {
    inc_password();
    display_password();
		return;
	}
	
}

// dis-armed
void sm_fuse_timer_submod3(unsigned char from, unsigned char to, enum task_events ev)
{
  CDBG("sm_fuse_timer_submod3 %bd %bd %bd\n", from, to, ev);

	if(ev == EV_KEY_V0) {
		roll_back(1); // 关闭所有传感器
		set_task(EV_KEY_V0);
		display_timer(DISPLAY_TIMER_DISARMED);
		return;
	}

}

// pre-detonate
void sm_fuse_timer_submod4(unsigned char from, unsigned char to, enum task_events ev)
{
  CDBG("sm_fuse_timer_submod4 %bd %bd %bd\n", from, to, ev);
	roll_back(0); // 关闭所有传感器，除了fuse
	set_task(EV_KEY_V0);
	display_timer(DISPLAY_TIMER_PREDETONATE);
}

/*
enum timer_display_state
{
  DISPLAY_TIMER_DISARMED,
  DISPLAY_TIMER_PREDETONATE,
};

static void display_timer(enum timer_display_state state)
{
  led_clear();
  led_set_code(5, 'P');
  led_set_code(4, 'L');
  led_set_code(3, '0');
  led_set_code(2, '-');
  switch(state) {
    case DISPLAY_TIMER_DISARMED:
      led_set_code(0, 'D');
      led_set_code(1, 'D');
      break;
    case DISPLAY_TIMER_PREDETONATE:
      led_set_code(0, 'C');
      led_set_code(1, 'C');
      break;
  }
}

static void stop_peripheral(void)
{
  thermo_enable(0);
  mpu_enable(0);
  hg_enable(0);
}

static void rollback_param(void)
{
  fuse_enable(0);
  stop_peripheral();
  lt_timer_reset();
}

static void display_prearm_step(unsigned char step)
{
  led_clear();
  led_set_code(5, 'P');
  led_set_code(4, 'L');
  led_set_code(3, '0');
  led_set_code(2, '-'); 
  
  led_set_code(1, (TIMER_PARAM_CHECK_CNT - step - 1) / 10 + 0x30);
  led_set_code(0, (TIMER_PARAM_CHECK_CNT - step - 1) % 10 + 0x30);  
}

static bit check_and_set_timer_param(unsigned char step)
{
  unsigned char val, thermo_hi, thermo_lo;

  display_prearm_step(step);
  CDBG("check_and_set_timer_param %bd\n", step);
  
  switch (step) {
    case TIMER_PARAM_CHECK_SET_MPU:
      // 如果打开了mpu，必须是好的
      val = rom_read(ROM_FUSE_MPU_ONOFF);
      if(val) {
        val = rom_read(ROM_MPU_GOOD);
        if(!val) {
          display_param_error(PARAM_ERROR_MPU_BAD);
          goto err;
        }
        mpu_enable(1);
      }
      break;
    case TIMER_PARAM_CHECK_SET_HG:
      // 如果打开了hg，必须是好的
      val = rom_read(ROM_FUSE_HG_ONOFF);
      if(val) {
        val = rom_read(ROM_HG_GOOD);
        if(!val) {
          display_param_error(PARAM_ERROR_HG_BAD);
          goto err;
        }
        hg_enable(1);
      }
      break;
    case TIMER_PARAM_CHECK_SET_THERMO:
      thermo_hi = rom_read(ROM_FUSE_THERMO_HI);
      thermo_lo = rom_read(ROM_FUSE_THERMO_LO);
      if(thermo_hi != THERMO_THRESHOLED_INVALID || thermo_lo != THERMO_THRESHOLED_INVALID) {
        current_temp = thermo_get_current();
        if(thermo_hi <= current_temp) {
          display_param_error(PARAM_ERROR_THERMO_TOO_HI);
          goto err;
        }
        if(thermo_lo >= current_temp) {
          display_param_error(PARAM_ERROR_THERMO_TOO_LOW);
          goto err;
        }
        
        if(thermo_hi != THERMO_THRESHOLED_INVALID && thermo_lo != THERMO_THRESHOLED_INVALID) {
          if(thermo_lo >= thermo_hi) {
            display_param_error(PARAM_ERROR_THERMO_HI_LESS_LO);
            goto err;
          }
        }
        thermo_enable(1);
        if(thermo_hi != THERMO_THRESHOLED_INVALID) {
          val = rom_read(ROM_THERMO_HI_GOOD);
          if(!val) {
            display_param_error(PARAM_ERROR_THERMO_HI_BAD);
            goto err;
          }
          thermo_hi_enable(1);
        }
        
        if(thermo_lo != THERMO_THRESHOLED_INVALID) {
          val = rom_read(ROM_THERMO_LO_GOOD);
          if(!val) {
            display_param_error(PARAM_ERROR_THERMO_LO_BAD);
            goto err;
          }
          thermo_lo_enable(1);
        }
      }
      break;
    case TIMER_PARAM_CHECK_SET_LT_TIME:
      // lt_timer时间必须在当前时间之后
      lt_timer_sync_from_rom();
      lt_timer_sync_to_rtc();
      if(!lt_timer_get_relative(1)) {
        display_param_error(PARAM_ERROR_LT_TIMER_OVERFLOW);
        goto err;
      } else {
        lt_timer_start_ram();
      }
      break;
    case TIMER_PARAM_CHECK_SET_FUSE:
      if((rom_read(ROM_FUSE0_BROKE_GOOD)
        || rom_read(ROM_FUSE1_BROKE_GOOD)) && rom_read(ROM_TRIPWIRE_GOOD)
      ) {
        fuse_enable(1);
      } else if(!rom_read(ROM_TRIPWIRE_GOOD)){
        display_param_error(PARAM_ERROR_TRIPWIRE_BAD);
        goto err;
      } else {
        display_param_error(PARAM_ERROR_FUSE_ERROR);
        goto err;
      }
      break;
    case TIMER_PARAM_DELAY0:
    case TIMER_PARAM_DELAY1:
    case TIMER_PARAM_DELAY2:
      break;
  }
  
  return 1;
err:
  return 0;
}

static void verify_password()
{
  unsigned char res;
  
  res = rom_read(ROM_FUSE_PASSWORD + password_index);
  
  verify_state &= ~(1 << password_index);
  
  if(res == password_content) {
    verify_state |= (1 << password_index);
    CDBG("verify_password %bd : %bd <->%bd success\n", password_index, res, password_content);
  } else {
    CDBG("verify_password %bd : %bd <->%bd failed\n", password_index, res, password_content);
  }
  return;
}

static void display_password()
{
  unsigned char i;
  led_clear();
  for( i = 0 ; i < 6 ; i ++ ) {
    if(i < password_index) {
      led_set_code(i, '-');
    } else if(i == password_index) {
      led_set_code(i, password_content + 0x30);
    } else {
      led_set_code(i, 'O');
    }
  }
}

static void inc_password(void)
{
  password_content = (++password_content) % 10;
}

void sm_fuse_timer(unsigned char from, unsigned char to, enum task_events ev)
{
  CDBG("sm_fuse_timer %bd %bd %bd\n", from, to, ev);
  
  // 从别的状态切过来
  if(get_sm_ss_state(to) == SM_FUSE_TIMER_INIT) {
    display_logo(DISPLAY_LOGO_TYPE_FUSE, 3);
    param_step = 0;
    return;
  }
  
  // 过1S进入prearm状态
  if(get_sm_ss_state(from) == SM_FUSE_TIMER_INIT
    && get_sm_ss_state(to) == SM_FUSE_TIMER_PREARMED && ev == EV_1S) {
    display_prearm_step(param_step);
    return;
  }

  // prearm状态，每1S启动一个传感器
  if(get_sm_ss_state(from) == SM_FUSE_TIMER_PREARMED
    && get_sm_ss_state(to) == SM_FUSE_TIMER_PREARMED && ev == EV_1S) {
    if(param_step != TIMER_PARAM_ERROR && param_step < TIMER_PARAM_CHECK_CNT) {
      if(!check_and_set_timer_param(param_step)) {
        rollback_param(); // 发生了错误，开始回滚
        param_step = TIMER_PARAM_ERROR;
        set_task(EV_FUSE_SEL0);
      } else {
        param_step ++; //设置下一个参数动作
        if(param_step >= TIMER_PARAM_CHECK_CNT) {
          set_task(EV_FUSE_SEL1); // 全部都通过了，进入armed
        }
      }
    }
    return;
  }
  
  // prearm状态，收到任何如下事件，都回滚
  if(get_sm_ss_state(to) == SM_FUSE_TIMER_PREARMED 
    && (ev == EV_COUNTER 
    || ev == EV_FUSE0_BROKE 
    || ev == EV_FUSE1_BROKE
    || ev == EV_ROTATE_MPU || ev == EV_DROP_MPU || ev == EV_ACC_MPU 
    || ev == EV_ROTATE_HG || ev == EV_FUSE_TRIPWIRE
    || ev == EV_THERMO_HI || ev == EV_THERMO_LO)) {
    rollback_param(); // 发生了错误，开始回滚
    param_step = TIMER_PARAM_ERROR;
    set_task(EV_FUSE_SEL0);
    return;
  }     
  
  // 进入armed状态，开始倒计时
  if(get_sm_ss_state(from) == SM_FUSE_TIMER_PREARMED 
    && get_sm_ss_state(to) == SM_FUSE_TIMER_ARMED && ev == EV_FUSE_SEL1) {
    lt_timer_display(1);
    password_index   = 0;
    password_content = 0;
    verify_state   = 0;
    power_reset_powersave_to();
    return;
  }
  
  // set0切换显示
  if(get_sm_ss_state(to) == SM_FUSE_TIMER_ARMED && ev == EV_KEY_SET_PRESS) {
    lt_timer_switch_display();
    power_reset_powersave_to();
    return;
  }
  
  if(get_sm_ss_state(to) == SM_FUSE_TIMER_ARMED && ev == EV_KEY_MOD_PRESS) {
    power_reset_powersave_to();
    return;
  }
  
  // mod0进入等待输入密码的状态
  if(get_sm_ss_state(from) == SM_FUSE_TIMER_ARMED
    && get_sm_ss_state(to) == SM_FUSE_TIMER_VERIFY 
    && ev == EV_KEY_MOD_PRESS) {
    password_index   = 5;   // current password index
    password_content = 0;   // current displayed password
    verify_state   &= 0xC0;   // verify state， last 6 bits is 0x3F is success， 2 bits retry count
    lt_timer_display(0);
    display_password(); // 使password_index作为password的index，为了减少ram使用
    power_reset_powersave_to();
    return;
  } 
    
  // 密码输入验证失败或者取消，返回倒计时
  if(get_sm_ss_state(from) == SM_FUSE_TIMER_VERIFY
    && get_sm_ss_state(to) == SM_FUSE_TIMER_ARMED 
    && (ev == EV_FUSE_SEL1 || ev == EV_KEY_SET_LPRESS)) {
    lt_timer_display(1);
    power_reset_powersave_to();
    return;
  }
  
  // 下一数字
  if(get_sm_ss_state(to) == SM_FUSE_TIMER_VERIFY 
    && get_sm_ss_state(from) == SM_FUSE_TIMER_VERIFY 
    && ev == EV_KEY_MOD_PRESS) {
    power_reset_powersave_to();
    if(password_index > 0) {
      verify_password();
      password_content = 0;
      password_index --;
      display_password(); // 使password_index作为password的index，为了减少ram使用。。。
    } else {
      // 校验结果，如果通过，发送EV_FUSE_SEL0
      // 如果失败小于3次，发送EV_FUSE_SEL1
      // 如果失败大于等于3次，发送EV_FUSE_SEL2
      verify_password();
      if((verify_state & 0x3F) == 0x3F) {
        CDBG("verify pass!\n");
        set_task(EV_FUSE_SEL0); // 校验通过了
      } else if((verify_state & 0xC0) == 0x80) {
        CDBG("verify failed, to detonate!\n");
        set_task(EV_FUSE_SEL2); // 校验多次不OK，挂了
      } else {
        verify_state += 0x40;
        CDBG("verify failed, count is %0bx!\n", (verify_state & 0xC0));
        set_task(EV_FUSE_SEL1);
      }
    }
    return;
  }
  
  // 输入数字
  if(get_sm_ss_state(to) == SM_FUSE_TIMER_VERIFY 
    && get_sm_ss_state(from) == SM_FUSE_TIMER_VERIFY 
    && ev == EV_KEY_SET_PRESS) {
    power_reset_powersave_to();
    inc_password();
    display_password();
  }
  
  // 每秒探测下是否进入powersave
  if(get_sm_ss_state(from) == SM_FUSE_TIMER_ARMED
    && get_sm_ss_state(to) == SM_FUSE_TIMER_ARMED && ev == EV_1S) {
    power_test_powersave_to();
    return;
  }
  
  // 被解除了
  if(get_sm_ss_state(from) == SM_FUSE_TIMER_VERIFY
    && get_sm_ss_state(to) == SM_FUSE_TIMER_DISARMED && ev == EV_FUSE_SEL0) {
    display_timer(DISPLAY_TIMER_DISARMED);
    rollback_param(); // 关闭所有外围电路，以及lt_timer
    set_task(EV_FUSE_SEL0);
    return;
  }
  
  // 从armed/verify 进入predetonate状态
  if(get_sm_ss_state(to) == SM_FUSE_TIMER_PREDETONATE
    &&(get_sm_ss_state(from) == SM_FUSE_TIMER_ARMED || get_sm_ss_state(from) == SM_FUSE_TIMER_VERIFY)
    && (ev == EV_COUNTER || ev == EV_FUSE_SEL2
    || ev == EV_FUSE0_BROKE 
    || ev == EV_FUSE1_BROKE
    || ev == EV_ROTATE_MPU || ev == EV_DROP_MPU || ev == EV_ACC_MPU 
    || ev == EV_ROTATE_HG || ev == EV_FUSE_TRIPWIRE
    || ev == EV_THERMO_HI || ev == EV_THERMO_LO)) {
    display_timer(DISPLAY_TIMER_PREDETONATE);
    stop_peripheral(); // 关闭所有外围电路，以及lt_timer
    lt_timer_reset();
    set_task(EV_FUSE_SEL0);
    return;
  } 

}
*/