#include "sm_fuse_test.h"
#include "debug.h"
#include "mod_common.h"
#include "alarm.h"
#include "lt_timer.h"
#include "fuse.h"
#include "tripwire.h"
#include "thermo.h"
#include "hg.h"
#include "gyro.h"
#include "clock.h"
#include "cext.h"

#define FUSE_TEST_TIMEO 5
#define HG_TEST_TIMEO   10
#define GYRO_TEST_TIMEO 10

static unsigned char in_testing;
static unsigned char begin_test_sec;
static unsigned char hg_state; // 高4位变化记录，低4位当前状态

enum fuse_display_state
{
	FUSE_DISPLAY_WAIT       = 0,
	FUSE_DISPLAY_TESTING_P1,
	FUSE_DISPLAY_TESTING_P2,	
	FUSE_DISPLAY_SHORT,
	FUSE_DISPLAY_BROKE,
	FUSE_DISPLAY_ERROR,	
	FUSE_DISPLAY_GOOD
};

enum tripwire_display_state
{
	TRIPWIRE_DISPLAY_WAIT       = 0,
	TRIPWIRE_DISPLAY_TESTING_P1,
	TRIPWIRE_DISPLAY_TESTING_P2,
	TRIPWIRE_DISPLAY_BROKE,
	TRIPWIRE_DISPLAY_ERROR,
	TRIPWIRE_DISPLAY_GOOD
};

enum thermo_display_state
{
	THERMO_DISPLAY_WAIT       = 0,
	THERMO_DISPLAY_TESTING_P1,
	THERMO_DISPLAY_TESTING_P2,
	THERMO_DISPLAY_TOO_HI,
	THERMO_DISPLAY_TOO_LO,
	THERMO_DISPLAY_ERROR,
	THERMO_DISPLAY_GOOD
};

enum hg_display_state
{
	HG_DISPLAY_WAIT       = 0,
	HG_DISPLAY_TESTING,
	HG_DISPLAY_ERROR,
	HG_DISPLAY_GOOD
};

enum gyro_display_state
{
	GYRO_DISPLAY_WAIT       = 0,
	GYRO_DISPLAY_TESTING_P1,
	GYRO_DISPLAY_TESTING_P2,
	GYRO_DISPLAY_TESTING_P3,
	GYRO_DISPLAY_ERROR,
	GYRO_DISPLAY_GOOD
};


static void display_fuse_state(unsigned char index, enum fuse_display_state state)
{
	
}

static void display_tripwire_state(enum tripwire_display_state state)
{
	
}

static void display_thermo_state(enum thermo_display_state state)
{
	
}

static void test_hg_state(unsigned char state)
{
	
}

static void display_hg_state(enum hg_display_state state, unsigned char mask)
{
	
}

static void display_gyro_state(enum gyro_display_state state)
{
	
}

void sm_fuse_test(unsigned char from, unsigned char to, enum task_events ev)
{
  CDBG("sm_fuse_test %bd %bd %bd\n", from, to, ev);

  // 按modset1进入fuse功能集合
  if(get_sm_ss_state(to) == SM_FUSE_TEST_INIT && ev == EV_KEY_MOD_SET_LPRESS) {
    alarm_switch_off();
    lt_timer_switch_on();
    display_logo(DISPLAY_LOGO_TYPE_FUSE, 0);
    return;
  }
  
  // 从别的状态切换过来，防止误操作
  if(get_sm_ss_state(from) == SM_FUSE_TEST_INIT 
    && get_sm_ss_state(to) == SM_FUSE_TEST_FUSE0_SHORT
    && (ev == EV_KEY_MOD_UP || ev == EV_KEY_SET_UP)) {
			display_fuse_state(0, FUSE_DISPLAY_WAIT);
    return;
  }
	
	// mod0 切换测试
	if(get_sm_ss_state(to) == SM_FUSE_TEST_FUSE0_BROKE && ev == EV_KEY_MOD_PRESS && in_testing == 0) {
		display_fuse_state(0, FUSE_DISPLAY_WAIT);
		return;
	}
	
	// mod0 切换测试
	if(get_sm_ss_state(to) == SM_FUSE_TEST_FUSE1_SHORT && ev == EV_KEY_MOD_PRESS && in_testing == 0) {
		display_fuse_state(1, FUSE_DISPLAY_WAIT);
		return;
	}
	
	// mod0 切换测试
	if(get_sm_ss_state(to) == SM_FUSE_TEST_FUSE1_BROKE && ev == EV_KEY_MOD_PRESS && in_testing == 0) {
		display_fuse_state(1, FUSE_DISPLAY_WAIT);
		return;
	}
	
	// set0 启动测试 fuse0 fuse1 short broke
	if((get_sm_ss_state(to) == SM_FUSE_TEST_FUSE0_SHORT 
		|| get_sm_ss_state(to) == SM_FUSE_TEST_FUSE0_BROKE
		|| get_sm_ss_state(to) == SM_FUSE_TEST_FUSE1_SHORT
		|| get_sm_ss_state(to) == SM_FUSE_TEST_FUSE1_BROKE
		)
		&& ev == EV_KEY_SET_PRESS && in_testing == 0) {
		fuse_enable(1);
		in_testing = 1;
		begin_test_sec = clock_get_sec();
		if(get_sm_ss_state(to) == SM_FUSE_TEST_FUSE0_SHORT 
			|| get_sm_ss_state(to) == SM_FUSE_TEST_FUSE0_BROKE )
			display_fuse_state(0, FUSE_DISPLAY_TESTING_P1);
		else {
			display_fuse_state(1, FUSE_DISPLAY_TESTING_P1);
		}
		return;
	}
	
	// 测试状态 fuse0 fuse1 short broke 
	if(  (get_sm_ss_state(to) == SM_FUSE_TEST_FUSE0_SHORT && (ev == EV_1S || ev == EV_FUSE0_SHORT))
		|| (get_sm_ss_state(to) == SM_FUSE_TEST_FUSE0_BROKE && (ev == EV_1S || ev == EV_FUSE0_BROKE))
		|| (get_sm_ss_state(to) == SM_FUSE_TEST_FUSE1_SHORT && (ev == EV_1S || ev == EV_FUSE1_SHORT))
		|| (get_sm_ss_state(to) == SM_FUSE_TEST_FUSE1_BROKE && (ev == EV_1S || ev == EV_FUSE1_BROKE))
		) {
		if(in_testing == 1 && ev == EV_1S) { // so far so good, continue!
			if(time_diff(clock_get_sec(), begin_test_sec) > FUSE_TEST_TIMEO) {
				in_testing = 2;
				if(get_sm_ss_state(to) == SM_FUSE_TEST_FUSE0_SHORT )
					fuse_set_fuse_short(0, 1);
				else if (get_sm_ss_state(to) == SM_FUSE_TEST_FUSE0_BROKE )
					fuse_set_fuse_broke(0, 1);
				else if (get_sm_ss_state(to) == SM_FUSE_TEST_FUSE1_SHORT )
					fuse_set_fuse_short(1, 1);
				else 
					fuse_set_fuse_broke(1, 1);
				display_fuse_state(1, FUSE_DISPLAY_TESTING_P2);
				begin_test_sec = clock_get_sec();
			}
		} else if(in_testing == 1 
			&& (ev == EV_FUSE0_SHORT
				|| ev == EV_FUSE0_BROKE
				|| ev == EV_FUSE1_SHORT
				|| ev == EV_FUSE1_BROKE)) { // bad: is short or broke
			in_testing = 0;
			fuse_enable(0);
			if(ev == EV_FUSE0_SHORT)
				display_fuse_state(0, FUSE_DISPLAY_SHORT);
			else if(ev == EV_FUSE0_BROKE)
				display_fuse_state(0, FUSE_DISPLAY_BROKE);
			else if (ev == EV_FUSE1_SHORT)
				display_fuse_state(1, FUSE_DISPLAY_SHORT);
			else
				display_fuse_state(1, FUSE_DISPLAY_BROKE);
		}else if(in_testing == 2 && ev == EV_1S) { // bad, 不响应
			if(time_diff(clock_get_sec(), begin_test_sec) > FUSE_TEST_TIMEO) {
				in_testing = 0;
				fuse_enable(0);
				display_fuse_state(0, FUSE_DISPLAY_ERROR);
				if(get_sm_ss_state(to) == SM_FUSE_TEST_FUSE0_SHORT) {
					fuse_set_fuse_short(0, 0);
					display_fuse_state(0, FUSE_DISPLAY_ERROR);
				} else if (get_sm_ss_state(to) == SM_FUSE_TEST_FUSE0_BROKE ) {
					fuse_set_fuse_broke(0, 0);
					display_fuse_state(0, FUSE_DISPLAY_ERROR);
				} else if (get_sm_ss_state(to) == SM_FUSE_TEST_FUSE1_SHORT ) {
					fuse_set_fuse_short(1, 0);
					display_fuse_state(1, FUSE_DISPLAY_ERROR);
				} else {
					fuse_set_fuse_broke(1, 0);
					display_fuse_state(1, FUSE_DISPLAY_ERROR);
				}
			}
		} else if(in_testing == 2 
		&&  (ev == EV_FUSE0_SHORT
				|| ev == EV_FUSE0_BROKE
				|| ev == EV_FUSE1_SHORT
				|| ev == EV_FUSE1_BROKE)) { // good!
			in_testing = 0;
			fuse_enable(0);
			if(ev == EV_FUSE0_SHORT) {
				fuse_set_fuse_short(0, 0);
				display_fuse_state(0, FUSE_DISPLAY_GOOD);	
			} else if(ev == EV_FUSE0_BROKE) {
				fuse_set_fuse_broke(0, 0);
				display_fuse_state(0, FUSE_DISPLAY_GOOD);	
			} else if(ev == EV_FUSE1_SHORT) {
				fuse_set_fuse_short(1, 0);
				display_fuse_state(1, FUSE_DISPLAY_GOOD);	
			} else {
				fuse_set_fuse_broke(1, 0);
				display_fuse_state(1, FUSE_DISPLAY_GOOD);	
			}
		}
	}
	
	// tripwire ，thermo hi， thermo lo测试
	// mod0 切换测试
	if((get_sm_ss_state(to) == SM_FUSE_TEST_TRIPWIRE 
		|| get_sm_ss_state(to) == SM_FUSE_TEST_THERMO_HI
		|| get_sm_ss_state(to) == SM_FUSE_TEST_THERMO_LO
		)
		&& ev == EV_KEY_MOD_PRESS && in_testing == 0) {
		if(get_sm_ss_state(to) == SM_FUSE_TEST_TRIPWIRE ) {
			display_tripwire_state(TRIPWIRE_DISPLAY_WAIT);
		} else {
			display_thermo_state(THERMO_DISPLAY_WAIT);
		}
		return;
	}
	
	// set0 开始测试
	if((get_sm_ss_state(to) == SM_FUSE_TEST_TRIPWIRE 
		|| get_sm_ss_state(to) == SM_FUSE_TEST_THERMO_HI
		|| get_sm_ss_state(to) == SM_FUSE_TEST_THERMO_LO
		)
		&& ev == EV_KEY_SET_PRESS && in_testing == 0) {
		in_testing = 1;
		begin_test_sec = clock_get_sec();
		if(get_sm_ss_state(to) == SM_FUSE_TEST_TRIPWIRE) {
			tripwire_enable(1);
			display_tripwire_state(TRIPWIRE_DISPLAY_TESTING_P1);
		} else if(get_sm_ss_state(to) == SM_FUSE_TEST_THERMO_HI) {
			thermo_hi_enable(1);
			display_thermo_state(THERMO_DISPLAY_TESTING_P1);
		} else {
			thermo_lo_enable(1);
			display_thermo_state(THERMO_DISPLAY_TESTING_P1);
		}
		return;
	}
	
	
	if((get_sm_ss_state(to) == SM_FUSE_TEST_TRIPWIRE 
		|| get_sm_ss_state(to) == SM_FUSE_TEST_THERMO_HI
		|| get_sm_ss_state(to) == SM_FUSE_TEST_THERMO_LO
		)
		&& ev == EV_1S) {
	  // P1 OK, into P2
		if(in_testing == 1 && time_diff(clock_get_sec(), begin_test_sec) > FUSE_TEST_TIMEO) {
			in_testing = 2;
			begin_test_sec = clock_get_sec();
			if(get_sm_ss_state(to) == SM_FUSE_TEST_TRIPWIRE) {
				tripwire_set_broke(1);
				display_tripwire_state(TRIPWIRE_DISPLAY_TESTING_P2);
			} else if(get_sm_ss_state(to) == SM_FUSE_TEST_THERMO_HI) {
				thermo_hi_set_hit(1);
				display_thermo_state(THERMO_DISPLAY_TESTING_P2);
			} else {
				thermo_lo_set_hit(1);
				display_thermo_state(THERMO_DISPLAY_TESTING_P2);
			}
		// P2 Failed，没响应？
		} else if(in_testing == 2 && time_diff(clock_get_sec(), begin_test_sec) > FUSE_TEST_TIMEO) {
			in_testing = 0;
			if(get_sm_ss_state(to) == SM_FUSE_TEST_TRIPWIRE) {
				tripwire_enable(0);
				display_tripwire_state(TRIPWIRE_DISPLAY_ERROR);
			} else if(get_sm_ss_state(to) == SM_FUSE_TEST_THERMO_HI) {
				thermo_hi_enable(0);
				display_thermo_state(THERMO_DISPLAY_ERROR);
			} else {
				thermo_lo_enable(0);
				display_thermo_state(THERMO_DISPLAY_ERROR);
			}
		}
		return;
	}
	
	if((get_sm_ss_state(to) == SM_FUSE_TEST_TRIPWIRE && ev == EV_TRIPWIRE)
		||(get_sm_ss_state(to) == SM_FUSE_TEST_THERMO_HI && ev == EV_THERMO_HI)
		||(get_sm_ss_state(to) == SM_FUSE_TEST_THERMO_LO && ev == EV_THERMO_LO)
		) {
		if(in_testing == 1) { // P1 Failed， 已经处于断开状态/温度激活状态？
			in_testing = 0;
			if(ev == EV_TRIPWIRE) {
				tripwire_enable(0);
				display_tripwire_state(TRIPWIRE_DISPLAY_BROKE);
			} else if(ev == EV_THERMO_HI){
				thermo_hi_enable(0);
				display_thermo_state(THERMO_DISPLAY_TOO_HI);
			} else {
				thermo_lo_enable(0);
				display_thermo_state(THERMO_DISPLAY_TOO_LO);
			}
		} else if(in_testing == 2) { // P2 OK
			in_testing = 0;
			if(ev == EV_TRIPWIRE) {
				tripwire_enable(0);
				display_tripwire_state(TRIPWIRE_DISPLAY_GOOD);
			} else if(ev == EV_THERMO_HI){
				thermo_hi_enable(0);
				display_thermo_state(THERMO_DISPLAY_GOOD);
			} else {
				thermo_lo_enable(0);
				display_thermo_state(THERMO_DISPLAY_GOOD);
			}
		}
		return;
	}
	
	// 进入HG测试
	if(get_sm_ss_state(to) == SM_FUSE_TEST_HG && ev == EV_KEY_MOD_PRESS && in_testing == 0) {
		display_hg_state(HG_DISPLAY_WAIT, 0);
		return;
	}
	// 启动HG测试
	if(get_sm_ss_state(to) == SM_FUSE_TEST_HG && ev == EV_KEY_SET_PRESS && in_testing == 0) {
		in_testing = 1;
		begin_test_sec = clock_get_sec();
		hg_enable(1);
		hg_state = hg_get_state();
		display_hg_state(HG_DISPLAY_TESTING, hg_state);
		return;
	}
	// 等待人去转
	if(get_sm_ss_state(to) == SM_FUSE_TEST_HG && ev == EV_ROTATE_HG && in_testing == 1) {
		test_hg_state(hg_get_state());
		display_hg_state(HG_DISPLAY_TESTING, hg_state);
		if((hg_state & 0xF0) == 0xF0) { // OK!
			hg_enable(0);
			display_hg_state(HG_DISPLAY_GOOD, 0);
			in_testing = 0;
		}
		return;
	}
	// 检查是否超时
	if(get_sm_ss_state(to) == SM_FUSE_TEST_HG && ev == EV_1S) {
		if(in_testing == 1 && time_diff(clock_get_sec(), begin_test_sec) > HG_TEST_TIMEO) {
			hg_enable(0);
			display_hg_state(HG_DISPLAY_ERROR, 0);
			in_testing = 0;
		}
		return;
	}
	
	
	// 进入Gyro测试
	if(get_sm_ss_state(to) == SM_FUSE_TEST_GYRO && ev == EV_KEY_MOD_PRESS && in_testing == 0) {
		display_gyro_state(GYRO_DISPLAY_WAIT);
		return;
	}
	// 启动Gyro测试
	if(get_sm_ss_state(to) == SM_FUSE_TEST_GYRO && ev == EV_KEY_SET_PRESS && in_testing == 0) {
		in_testing = 1;
		begin_test_sec = clock_get_sec();
		gyro_enable(1);
		display_gyro_state(GYRO_DISPLAY_TESTING_P1);
		return;
	}
	// 等待人去晃
	if(get_sm_ss_state(to) == SM_FUSE_TEST_GYRO && ev == EV_ACC_GYRO && in_testing == 1) {
		if(gyro_test_acc_event()) { // OK!
			display_gyro_state(GYRO_DISPLAY_TESTING_P2);
			in_testing = 2;
			begin_test_sec = clock_get_sec();
		}
		return;
	}
	// 等待人丢
	if(get_sm_ss_state(to) == SM_FUSE_TEST_GYRO && ev == EV_ACC_GYRO && in_testing == 2) {
		if(gyro_test_drop_event()) { // OK!
			display_gyro_state(GYRO_DISPLAY_TESTING_P3);
			in_testing = 3;
			begin_test_sec = clock_get_sec();
		}
		return;
	}
	// 等待人去转
	if(get_sm_ss_state(to) == SM_FUSE_TEST_GYRO && ev == EV_ROTATE_GYRO && in_testing == 3) {
		if(gyro_test_rotate_event()) { // OK!
			display_gyro_state(GYRO_DISPLAY_GOOD);
			in_testing = 0;
		}
		return;
	}
	// 检查是否超时
	if(get_sm_ss_state(to) == SM_FUSE_TEST_GYRO && ev == EV_1S) {
		if(time_diff(clock_get_sec(), begin_test_sec) > GYRO_TEST_TIMEO) {
			gyro_enable(0);
			display_gyro_state(GYRO_DISPLAY_ERROR);
			in_testing = 0;
		}
		return;
	}
}