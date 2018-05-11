#include "sm_fuse_test.h"
#include "debug.h"
#include "mod_common.h"
#include "alarm.h"
#include "lt_timer.h"
#include "fuse.h"
#include "thermo.h"
#include "hg.h"
#include "mpu.h"
#include "clock.h"
#include "cext.h"
#include "led.h"
#include "rom.h"
#include "rtc.h"
#include "power.h"

#define FUSE_TEST_TIMEO 5
#define FUSE_MANUAL_TIMEO   30

const char * code sm_fuse_test_ss_name[] = 
{
  "SM_FUSE_TEST_INIT",
  "SM_FUSE_TEST_BROKE",
  "SM_FUSE_TEST_THERMO_HI_SET",
  "SM_FUSE_TEST_THERMO_HI",
  "SM_FUSE_TEST_THERMO_LO_SET",
  "SM_FUSE_TEST_THERMO_LO",
  "SM_FUSE_TEST_HG_SET",
  "SM_FUSE_TEST_HG",
  "SM_FUSE_TEST_MPU_SET",
  "SM_FUSE_TEST_MPU",
  NULL
};


#define test_stage lpress_start   // use lpress_start 作为测试状态指示
#define test_to    last_display_s // use last_display_s作为超时起始时间
#define hg_mask    common_state   // use common_state 作为hg变化指示
#define set_value  common_state   // use common_state 作为设置thermo，hg，mpu的临时变量
#define lpress_lock lpress_lock_year_hour // use lpress_lock_year_hour 作为inc_only的状态指示

enum sm_fuse_err
{
	FUSE_ERR_OK = 0, // 测试成功
	FUSE_ERR_WAIT,   // 测试中
	FUSE_ERR_MANUAL, // 需要手工介入（晃动）
	FUSE_ERR_BROKE,  // 测试失败（fuse断线、tripwire断线、hg/mpu/thermo加电就报警、）
	FUSE_ERR_NOT_RESPONSE, // 无响应
};

void sm_fuse_test_init(unsigned char from, unsigned char to, enum task_events ev)
{
	CDBG("sm_fuse_test_init %bd %bd %bd\n", from, to, ev);
	clock_display(0);
	display_logo(DISPLAY_LOGO_TYPE_FUSE, 0);
	alarm_switch_off();
	lt_timer_switch_on();
	rtc_set_lt_timer(1);
	power_5v_enable(0);
	test_stage = 0;
	test_to    = 0;
	if(ev == EV_KEY_V0) { // 从拆除切换过来
		set_task(EV_KEY_MOD_UP);
	}
}
// hg_mask : 低4位是上一次hg的状态，高4位：上一次hg状态和本次不同设置为1，相同不动
static unsigned char update_hg_mask(unsigned char state, unsigned char mask)
{
	unsigned char val;
	CDBG("before update_hg_mask state = %bx, mask = %bx\n", state, mask);
	val = (state ^ mask) & 0xF; // 变化的位设置为1
	val = val << 4; // 低4位移到高4位
	mask |= val;    // 与原有高4位取或
	mask &= 0xF0;   // 清除低4位
	mask |= (state & 0xF); // 保存低4位为新状态
	CDBG("after update_hg_mask state = %bx, mask = %bx\n", state, mask);
	return mask;
}

static void display_test(unsigned char what, unsigned char stage, unsigned char val, unsigned char errcode)
{
	char tmp;
	CDBG("display_fuse_test %bd %bd %bd，%bd\n", what, stage, val, errcode);
	
	led_clear();
	
	led_set_dp(4);
	led_set_dp(3);
	
	if(errcode == FUSE_ERR_OK) {
		led_set_code(3, 'P');
		led_set_code(2, 'A');			
		led_set_code(1, 'S');
		led_set_code(0, 'S');
	} else if(errcode == FUSE_ERR_BROKE){
		led_set_code(3, stage + 0x30);
		led_set_code(2, '-');			
		led_set_code(1, 'B');
		led_set_code(0, 'R');
	} else if(errcode == FUSE_ERR_NOT_RESPONSE) {
		led_set_code(3, stage + 0x30);
		led_set_code(2, '-');			
		led_set_code(1, 'R');
		led_set_code(0, 'E');
	}
	
	switch(what) {
		case IS_FUSE0: // FU
		case IS_FUSE1:
		case IS_TRIPWIRE:
			led_set_code(5, 'F');
			led_set_code(4, 'U');
			if(what == IS_FUSE0) {
				val = '0';
			}else if(what == IS_FUSE1) {
				val = '1';
			} else {
				val = 'T';
			}
			if(errcode == FUSE_ERR_WAIT) {
				led_set_code(3, stage + 0x30);
				led_set_code(2, val); // fuse0, fuse1 or tripwire			
				led_set_code(1, '-');
				led_set_code(0, '-');
				led_set_blink(2);
				led_set_blink(1);
				led_set_blink(0);				
			}
			break;
		case IS_THERMO_HI:
		case IS_THERMO_LO:
			led_set_code(5, 'T');
			if(what == IS_THERMO_HI) {
				led_set_code(4, 'H');
			} else {
				led_set_code(4, 'L');
			}
			if(errcode == FUSE_ERR_WAIT) { // 显示温度变化
				led_set_code(3, stage + 0x30);
				tmp = (char) val;
				if(tmp < 0) {
					led_set_code(2, '-');	
					tmp = 0 - tmp;
				}		
				led_set_code(1, (tmp / 10) + 0x30);
				led_set_code(0, (tmp % 10) + 0x30);			
			}
			break;
		case IS_HG:
		case IS_MPU:
			if(what == IS_HG) {
				led_set_code(5, 'H');
				led_set_code(4, 'G');
			} else {
				led_set_code(5, 'P');
				led_set_code(4, 'U');
			}
			if(errcode == FUSE_ERR_WAIT) { 
				led_set_code(3, stage + 0x30);
				led_set_code(2, '-');			
				led_set_code(1, '-');
				led_set_code(0, '-');
				led_set_blink(2);
				led_set_blink(1);
				led_set_blink(0);			
			} else if(errcode == FUSE_ERR_MANUAL) {
				led_set_code(3, 'S'); // shake me !
				led_set_code(2, '-');
				// IS_HG显示hg_mask变化, IS_MPU时为敏感度
				led_set_code(1, (val / 16) + 0x30); 
				led_set_code(0, (val % 16) + 0x30);	
			}
			break;
	}
}

static void update_set(unsigned char what)
{
	char val;
	switch(what) {
		case IS_THERMO_HI: // TH
		case IS_THERMO_LO: // TL
			val = (char)set_value;
			if(set_value == THERMO_THRESHOLD_INVALID) {
				led_set_code(2, 'O');		
				led_set_code(1, 'F');
				led_set_code(0, 'F');
			} else {
				if(val < 0) {
					led_set_code(2, '-');
					val = 0 - val;
				}
				led_set_code(1, val / 10 + 0x30);
				led_set_code(0, val % 10 + 0x30); 
			}
			break;
		case IS_HG:        // HG
			if(set_value) {
				led_set_code(2, LED_CODE_BLACK);
				led_set_code(1, 'O');
				led_set_code(0, 'N');
			} else {
				led_set_code(2, 'O');		
				led_set_code(1, 'F');
				led_set_code(0, 'F');
			}
			break;
		case IS_MPU:       // PU
			if(set_value == MPU_THRESHOLD_INVALID) {
				led_set_code(2, 'O');		
				led_set_code(1, 'F');
				led_set_code(0, 'F');
			} else {
				led_set_code(1, set_value / 10 + 0x30);
				led_set_code(0, set_value % 10 + 0x30); 
			}
			break;
	}
}

// thermo_hi,thermo_lo,hg,mpu
static void enter_set(unsigned char what)
{
	CDBG("display_set %bd\n", what);
	led_clear();
	led_set_dp(4);
	led_set_dp(3);
	led_set_blink(2);
	led_set_blink(1);
	led_set_blink(0);
	switch(what) {
		case IS_THERMO_HI: // TH
		case IS_THERMO_LO: // TL
			led_set_code(5, 'T');
			if(what == IS_THERMO_HI) {
				led_set_code(4, 'H');
			} else {
				led_set_code(4, 'L');
			}
			set_value = rom_read(what == IS_THERMO_HI?
				ROM_FUSE_THERMO_HI : ROM_FUSE_THERMO_LO);
			break;
		case IS_HG:        // HG
			led_set_code(5, 'H');
			led_set_code(4, 'G');
			set_value = rom_read(ROM_FUSE_HG_ONOFF);
			break;
		case IS_MPU:       // PU
			led_set_code(5, 'P');
			led_set_code(4, 'U');
			set_value = rom_read(ROM_FUSE_MPU);
			break;
	}
	update_set(what);
}

static void inc_only(unsigned char what)
{

	CDBG("inc_only %bd\n", what);

  if(!lpress_lock) {
		lpress_lock = 1;
		led_clr_blink(2);
		led_clr_blink(1);
		led_clr_blink(0);	
	}
	
  switch (what) {
		case IS_THERMO_HI:
		case IS_THERMO_LO:
			set_value = thermo_threshold_inc(set_value);
      break;
		case IS_HG:
			set_value = !set_value;
			break;
		case IS_MPU:
			set_value = mpu_threshold_inc(set_value);
			break;
	}
	update_set(what);
}

static void write_only(unsigned char what)
{
	CDBG("write_only %bd\n", what);
	
  if(lpress_lock) {
		lpress_lock = 0;
		led_clr_blink(2);
		led_clr_blink(1);
		led_clr_blink(0);	
	}
	
  switch (what) {
		case IS_THERMO_HI:
			rom_write(ROM_FUSE_THERMO_HI, set_value);
			break;
		case IS_THERMO_LO:
			rom_write(ROM_FUSE_THERMO_LO, set_value);
      break;
		case IS_HG:
			rom_write(ROM_FUSE_HG_ONOFF, set_value);
			break;
		case IS_MPU:
			rom_write(ROM_FUSE_MPU, set_value);
			break;
	}
}

static void inc_write(unsigned char what)
{
	CDBG("inc_write %bd\n", what);
	inc_only(what);
	write_only(what);
}

static void sm_fuse_test_thermo(unsigned char what, enum task_events ev)
{
	char val;
	if(ev == EV_KEY_MOD_PRESS) {
		// 初始化
		test_stage = 0;
		test_to    = 0;
		thermo_enable(0);
		display_test(what, test_stage, 0, FUSE_ERR_WAIT);
		return;
	}
	
	if(ev == EV_KEY_SET_PRESS && test_stage == 0) {
		// start test
		if(what == IS_THERMO_HI)
			val = rom_read(ROM_FUSE_THERMO_HI);
		else
			val = rom_read(ROM_FUSE_THERMO_LO);
		if(val != THERMO_THRESHOLD_INVALID) {
			// 开始测试
			test_stage = 1;
			test_to = clock_get_sec_256();
			thermo_enable(1);
			display_test(what, test_stage, 
				what == IS_THERMO_HI ? thermo_hi_threshold_get() : thermo_lo_threshold_get(), 
				FUSE_ERR_WAIT);
		} else {
			// 不用测试了啊
			display_test(what, test_stage, 
				what == IS_THERMO_HI ? thermo_hi_threshold_get() : thermo_lo_threshold_get(), 
				FUSE_ERR_OK);
		}
		return;
	}
	
	if(ev == EV_THERMO_HI || ev == EV_THERMO_LO) {
		if(test_stage == 2) {
			// 测试成功
			display_test(what, test_stage, 
				what == IS_THERMO_HI ? thermo_hi_threshold_get() : thermo_lo_threshold_get(), 
				FUSE_ERR_OK);
			thermo_enable(0);
			test_stage = 0;
		} else if(test_stage == 1) {
			// 错误：一加电就产生事件
			display_test(what, test_stage, 
				what == IS_THERMO_HI ? thermo_hi_threshold_get() : thermo_lo_threshold_get(), 
				FUSE_ERR_BROKE);
			thermo_enable(0);
			test_stage = 0;
		}
		return;
	}
	
	if(ev == EV_1S) {
		if(test_stage == 1) {
			if(time_diff_now(test_to) > FUSE_TEST_TIMEO) {
				test_stage = 2;
				test_to = clock_get_sec_256();
				display_test(what, test_stage, 
					what == IS_THERMO_HI ? thermo_hi_threshold_get() : thermo_lo_threshold_get(), 
					FUSE_ERR_WAIT);
			}
		} else if(test_stage == 2) {
			if(what == IS_THERMO_HI) {
				if(!thermo_hi_threshold_reach_bottom()) {
					thermo_hi_threshold_dec();
					display_test(IS_THERMO_HI, test_stage, thermo_hi_threshold_get(), FUSE_ERR_WAIT);
				} else {
					display_test(IS_THERMO_HI, test_stage, thermo_hi_threshold_get(), FUSE_ERR_NOT_RESPONSE);
					thermo_enable(0);
					test_stage = 0;
				}
			} else if(what == IS_THERMO_LO) {
				if(!thermo_lo_threshold_reach_top()) {
					thermo_lo_threshold_inc();
					display_test(IS_THERMO_LO, test_stage, thermo_lo_threshold_get(), FUSE_ERR_WAIT);
				} else {
					display_test(IS_THERMO_LO, test_stage, thermo_lo_threshold_get(), FUSE_ERR_NOT_RESPONSE);
					thermo_enable(0);
					test_stage = 0;
				}
			}
		}
		return;
	}
}

static void sm_fuse_set(unsigned char what, enum task_events ev)
{
	if(ev == EV_KEY_MOD_PRESS) {
		enter_set(what);
		lpress_start = 0;
		return;
	}
	
	if(ev == EV_KEY_SET_PRESS) {
		inc_write(what);
		return;
	}
	
	if(ev == EV_KEY_SET_LPRESS) {
    if((lpress_start % LPRESS_INC_DELAY) == 0) {
      inc_only(IS_MIN);
    }
    lpress_start++;
    if(lpress_start == LPRESS_INC_OVERFLOW) lpress_start = 0;
		return;
	}
	
	if(ev == EV_KEY_SET_UP) {
		write_only(what);
		lpress_start = 0;
		return;
	}
}

// test_stage: 
// 0: 未开始
// 1: fuse 加电
// 2: fuse 设置 fuse0 broke
// 3: fuse 设置 fuse1 broke
// 4：fuse 设置 tripwire broke
void sm_fuse_test_submod0(unsigned char from, unsigned char to, enum task_events ev)
{
	CDBG("sm_fuse_test_submod0 %bd %bd %bd\n", from, to, ev);
	
	if(ev == EV_KEY_MOD_UP || ev ==  EV_KEY_MOD_PRESS ) {
		// 初始化
		test_stage = 0;
		test_to    = 0;
		fuse_enable(0);
		display_test(IS_FUSE0, test_stage, 0, FUSE_ERR_WAIT);
		return;
	} 

	if(ev == EV_KEY_SET_PRESS && test_stage == 0) {
		// start test
		test_stage = 1;
		test_to = clock_get_sec_256();
		fuse_enable(1);
		display_test(IS_FUSE0, test_stage, 0, FUSE_ERR_WAIT);
		return;
	} 
	
	if(ev == EV_FUSE0_BROKE) {
		// update test state
		if(test_stage == 2) {
			// 正确：加电后，设置fuse0 broke，应该收到EV_FUSE0_BROKE事件
			test_stage = 3;
			fuse_set_fuse_broke(0, 0);
			fuse_set_fuse_broke(1, 1);
			test_to = clock_get_sec_256();
			display_test(IS_FUSE0, test_stage, 0, FUSE_ERR_WAIT);
		} else {
			// 错误：非正确时机收到EV_FUSE0_BROKE事件
			display_test(IS_FUSE0, test_stage, 0, FUSE_ERR_BROKE);
			fuse_enable(0);
			test_stage = 0;
		}
		return;
	}
	
	if(ev == EV_FUSE1_BROKE) {
		// update test state
		if(test_stage == 3) {
			// 正确：加电后，设置fuse1 broke，应该收到EV_FUSE1_BROKE事件
			test_stage = 4;
			fuse_set_fuse_broke(1, 0);
			fuse_set_tripwire_broke(1);
			test_to = clock_get_sec_256();
			display_test(IS_FUSE1, test_stage, 0, FUSE_ERR_WAIT);
		} else {
			// 错误：非正确时机收到EV_FUSE1_BROKE事件
			display_test(IS_FUSE1, test_stage, 0, FUSE_ERR_BROKE);
			fuse_enable(0);
			test_stage = 0;
		}
		return;
	}
	
	if(ev == EV_FUSE_TRIPWIRE) {
		// update test state
		if(test_stage == 4) {
			// 正确：加电后，设置tripwire broke，应该收到EV_FUSE_TRIPWIRE事件
			// 测试全部结束
			fuse_enable(0);
			test_stage = 0;
			display_test(IS_TRIPWIRE, test_stage, 0, FUSE_ERR_OK);
		} else {
			// 错误：非正确时机收到EV_FUSE_TRIPWIRE事件
			display_test(IS_TRIPWIRE, test_stage, 0, FUSE_ERR_BROKE);
			fuse_enable(0);
			test_stage = 0;
		}
		return;
	}
	
	if(ev == EV_1S ) {
		// update test state
		if(test_stage == 1) {
			if(time_diff_now(test_to) > FUSE_TEST_TIMEO) {
				test_stage = 2;
				fuse_set_fuse_broke(0, 1);
				test_to = clock_get_sec_256();
				display_test(IS_FUSE0, test_stage, 0, FUSE_ERR_WAIT);
			}
		} else if(test_stage == 2) {
			if(time_diff_now(test_to) > FUSE_TEST_TIMEO) {
				// 错误：设置了 fuse0 broke，没有响应
				display_test(IS_FUSE0, test_stage, 0, FUSE_ERR_NOT_RESPONSE);
				fuse_enable(0);
				test_stage = 0;
			}
		} else if(test_stage == 3) {
			if(time_diff_now(test_to) > FUSE_TEST_TIMEO) {
				// 错误：设置了 fuse1 broke，没有响应
				display_test(IS_FUSE1, test_stage, 0, FUSE_ERR_NOT_RESPONSE);
				fuse_enable(0);
				test_stage = 0;
			}
		} else if(test_stage == 4) {
			if(time_diff_now(test_to) > FUSE_TEST_TIMEO) {
				// 错误：设置了 tripwire broke，没有响应
				display_test(IS_TRIPWIRE, test_stage, 0, FUSE_ERR_NOT_RESPONSE);
				fuse_enable(0);
				test_stage = 0;
			}
		}
		return;
	}
}

void sm_fuse_test_submod1(unsigned char from, unsigned char to, enum task_events ev)
{
	CDBG("sm_fuse_test_submod1 %bd %bd %bd\n", from, to, ev);
	sm_fuse_set(IS_THERMO_HI, ev);
}


// test_stage: 
// 0: 未开始
// 1: thermo 加电，hi enable/lo enable
// 2: 开始下调hi/上调lo
void sm_fuse_test_submod2(unsigned char from, unsigned char to, enum task_events ev)
{
	CDBG("sm_fuse_test_submod2 %bd %bd %bd\n", from, to, ev);
	sm_fuse_test_thermo(IS_THERMO_HI, ev);
}

void sm_fuse_test_submod3(unsigned char from, unsigned char to, enum task_events ev)
{
	CDBG("sm_fuse_test_submod3 %bd %bd %bd\n", from, to, ev);
	sm_fuse_set(IS_THERMO_LO, ev);
}

void sm_fuse_test_submod4(unsigned char from, unsigned char to, enum task_events ev)
{
	CDBG("sm_fuse_test_submod4 %bd %bd %bd\n", from, to, ev);
	sm_fuse_test_thermo(IS_THERMO_LO, ev);
}

void sm_fuse_test_submod5(unsigned char from, unsigned char to, enum task_events ev)
{
	CDBG("sm_fuse_test_submod5 %bd %bd %bd\n", from, to, ev);
	sm_fuse_set(IS_HG, ev);
}

static void sm_fuse_test_hg_mpu(unsigned char what, enum task_events ev)
{
	unsigned char val;
		
	if(ev == EV_KEY_MOD_PRESS) {
		// 初始化
		test_to    = 0;
		test_stage = 0;
		hg_mask = 0;
		if(what == IS_HG) {
			hg_enable(0);
		} else {
			mpu_enable(0);
		}
		display_test(what, test_stage, hg_mask, FUSE_ERR_WAIT);
		return;
	}
	
	if(ev == EV_KEY_SET_PRESS && test_stage == 0) {
		val = (what == IS_HG ? rom_read(ROM_FUSE_HG_ONOFF) : rom_read(ROM_FUSE_MPU));
		if(what == IS_HG && val == 1) {
			// 开始hg 测试
			test_stage = 1;
			test_to = clock_get_sec_256();
			hg_enable(1);
			hg_mask = hg_get_state() & 0xF; // 低4位是状态，高4位清0
			display_test(IS_HG, test_stage, hg_mask, FUSE_ERR_WAIT);
		} else if(what == IS_MPU && val != MPU_THRESHOLD_INVALID){
			test_stage = 1;
			test_to = clock_get_sec_256();
			hg_enable(1);
			hg_mask = mpu_threshold_get();
			display_test(IS_MPU, test_stage, hg_mask, FUSE_ERR_WAIT);
		} else {
			// 不用测试了
			display_test(what, test_stage, hg_mask, FUSE_ERR_OK);
		}
		return;
	}
	
	if(ev == EV_ROTATE_HG) {
		if(test_stage == 2) {
			hg_mask = update_hg_mask(hg_get_state(), hg_mask);
			display_test(IS_HG, test_stage, hg_mask, FUSE_ERR_WAIT);
			if(hg_mask == 0xF0) {
				// 测试成功
				display_test(IS_HG, test_stage, hg_mask, FUSE_ERR_OK);
				test_stage = 0;
				hg_enable(0);
			}
		} else if(test_stage == 1) {
			// 错误，加电未晃动就有EV_ROTATE_HG产生
				display_test(IS_HG, test_stage, hg_mask, FUSE_ERR_BROKE);
				test_stage = 0;
				hg_enable(0);
		}
		return;
	}
	
	if(ev == EV_MOT_MPU) {
		if(test_stage == 2) {
			// 测试成功
			display_test(IS_MPU, test_stage, hg_mask, FUSE_ERR_OK);
			test_stage = 0;
			hg_enable(0);
		} else if(test_stage == 1) {
			// 错误，加电未晃动就有EV_MOT_MPU产生
			display_test(IS_MPU, test_stage, hg_mask, FUSE_ERR_BROKE);
			test_stage = 0;
			hg_enable(0);
		}
		return;
	}
	
	if(ev == EV_1S) {
		if(test_stage == 1) {
			if(time_diff_now(test_to) > FUSE_TEST_TIMEO) {
				// 加电等待一段时间，不应该有EV_ROTATE_HG/EV_ROTATE_MPU产生，开始测试晃动
				test_stage = 2;
				test_to = clock_get_sec_256();
				display_test(what, test_stage, hg_mask, FUSE_ERR_MANUAL);
			}
		} else if(test_stage == 2) {
			if(time_diff_now(test_to) > FUSE_MANUAL_TIMEO) {
				// 错误：等待晃动超时
				display_test(what, test_stage, hg_mask, FUSE_ERR_NOT_RESPONSE);
				test_stage = 0;
				hg_enable(0);
			}
		}
		return;
	}
}

// test_stage: 
// 0: 未开始
// 1: hg/MPU 加电
// 2: 等待晃动
void sm_fuse_test_submod6(unsigned char from, unsigned char to, enum task_events ev)
{
	CDBG("sm_fuse_test_submod6 %bd %bd %bd\n", from, to, ev);
	sm_fuse_test_hg_mpu(IS_HG, ev);
}

void sm_fuse_test_submod7(unsigned char from, unsigned char to, enum task_events ev)
{
	CDBG("sm_fuse_test_submod7 %bd %bd %bd\n", from, to, ev);
	sm_fuse_set(IS_MPU, ev);
}


void sm_fuse_test_submod8(unsigned char from, unsigned char to, enum task_events ev)
{
	CDBG("sm_fuse_test_submod8 %bd %bd %bd\n", from, to, ev);
	sm_fuse_test_hg_mpu(IS_MPU, ev);
}
