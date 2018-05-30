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
	TIMER_ERR_LT_TIMER_TOO_LONG
};

enum timer_display_state
{
  DISPLAY_TIMER_DISARMED,
  DISPLAY_TIMER_PREDETONATE,
};


static unsigned char check_and_set(unsigned char step)
{
	unsigned char ret = TIMER_ERR_OK;
	unsigned char val, val1;
	switch(step) {
		case TIMER_ARM_FUSE: 
			fuse_enable(1);
			break;
		case TIMER_ARM_MPU:
			val = rom_read(ROM_FUSE_MPU);
			mpu_enable(val != MPU_THRESHOLD_INVALID);
			break;
		case TIMER_ARM_HG:
			val = rom_read(ROM_FUSE_HG_ONOFF);
			hg_enable(val == 1);
			break;
		case TIMER_ARM_THERMO:
			val = rom_read(ROM_FUSE_THERMO_HI);
			val1 = rom_read(ROM_FUSE_THERMO_LO);
			if(val != THERMO_THRESHOLD_INVALID || val1 != THERMO_THRESHOLD_INVALID) {
				if(val != THERMO_THRESHOLD_INVALID && val1 != THERMO_THRESHOLD_INVALID) {
					if((char)val <= (char)val1) {
						ret = TIMER_ERR_THERMO_LO_GE_HI;
						break;
					}
				}
				thermo_enable(1);
			}
			break;
		case TIMER_ARM_LT_TIMER:
			lt_timer_load_from_rom();
      lt_timer_sync_to_rtc();
			val = lt_timer_get_relative(1);
			if(val == 1) {
				ret = TIMER_ERR_LT_TIMER_TOO_CLOSE;
        break;
			} else if(val == 2) {
				ret = TIMER_ERR_LT_TIMER_TOO_LONG;
        break;
			} else {
				lt_timer_start_ram();
			}
      break;
		case TIMER_ARM_DELAY0:
		case TIMER_ARM_DELAY1:
		case TIMER_ARM_DELAY2:
			break;
	}
	return ret;
}

static void roll_back(bit include_fuse)
{
	CDBG("roll_back include_fuse = %bu\n", include_fuse ? 1 : 0);
  thermo_enable(0);
  mpu_enable(0);
  hg_enable(0);
  lt_timer_reset();
	if(include_fuse) {
		fuse_enable(0);
	}
}
/*
  TIMER_ARM_FUSE = 0, 
  TIMER_ARM_MPU,
  TIMER_ARM_HG,
  TIMER_ARM_THERMO,
  TIMER_ARM_LT_TIMER
  TIMER_ARM_DELAY0,
  TIMER_ARM_DELAY1,
  TIMER_ARM_DELAY2,
*/

static void display_prearm(unsigned char step, unsigned char err)
{
  led_clear();
	
	switch(step) {
		case TIMER_ARM_FUSE:
			led_set_code(5, 'F');
			led_set_code(4, 'U');
		break;
		case TIMER_ARM_MPU:
			led_set_code(5, 'P');
			led_set_code(4, 'U');
		break;
		case TIMER_ARM_HG:
			led_set_code(5, 'H');
			led_set_code(4, 'G');
		break;
		case TIMER_ARM_THERMO:
			led_set_code(5, 'T');
			led_set_code(4, 'H');
		break;
		case TIMER_ARM_LT_TIMER:
			led_set_code(5, 'L');
			led_set_code(4, 'T');
		break;
		case TIMER_ARM_DELAY0:
		case TIMER_ARM_DELAY1:
		case TIMER_ARM_DELAY2:
			led_set_code(5, (step - TIMER_ARM_LT_TIMER) / 10 + 0x30);
			led_set_code(4, (step - TIMER_ARM_LT_TIMER) % 10 + 0x30);
	}
  led_set_code(3, '-');
	led_set_code(2, '-');
  led_set_code(1, (err / 10) + 0x30);
  led_set_code(0, (err % 10) + 0x30);
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
    CDBG("verify_password %bu : %bu <->%bu success\n", password_index, res, password_content);
  } else {
    CDBG("verify_password %bu : %bu <->%bu failed\n", password_index, res, password_content);
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
  CDBG("sm_fuse_timer_init %bu %bu %bu\n", from, to, ev);
	display_logo(DISPLAY_LOGO_TYPE_FUSE, 3);
	next_arm_step = 0;
	in_rollback   = 1;
}

// pre-arm
void sm_fuse_timer_submod0(unsigned char from, unsigned char to, enum task_events ev)
{
	unsigned char err;
  CDBG("sm_fuse_timer_submod0 %bu %bu %bu\n", from, to, ev);
	
	if(ev == EV_KEY_MOD_UP) {
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
  CDBG("sm_fuse_timer_submod1 %bu %bu %bu\n", from, to, ev);
	
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
  CDBG("sm_fuse_timer_submod2 %bu %bu %bu\n", from, to, ev);
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
					CDBG("verify failed, count is 0x%02bx!\n", (verify_state & 0xC0));
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
	
	if(ev == EV_1S) {
		
		return;
	}
}

// dis-armed
void sm_fuse_timer_submod3(unsigned char from, unsigned char to, enum task_events ev)
{
  CDBG("sm_fuse_timer_submod3 %bu %bu %bu\n", from, to, ev);

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
  CDBG("sm_fuse_timer_submod4 %bu %bu %bu\n", from, to, ev);
	roll_back(0); // 关闭所有传感器，除了fuse
	display_timer(DISPLAY_TIMER_PREDETONATE);
	set_task(EV_KEY_V0);
}
