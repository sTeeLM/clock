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
#include "led.h"

#define FUSE_TEST_TIMEO 5
#define HG_TEST_TIMEO   60
#define GYRO_TEST_TIMEO 60


static unsigned char in_testing;
static unsigned char begin_test_sec;
static unsigned char hg_state;
static unsigned char hg_state_mask;


static void test_hg_state_mask(unsigned char state)
{
  char val = hg_state ^ state;
  
  CDBG("test_hg_state_mask pld_hg_state = %bx state = %bx hg_state_mask = %bx\n", hg_state, state, hg_state_mask);
  
  hg_state |= (state & 0xF);
  
  if((val & 1)) {
    if(hg_state & 0x10) {
      hg_state_mask |= 1;
    } else {
      hg_state |= 0x10;
    }
  }

  if((val & 2)) {
    if(hg_state & 0x20) {
      hg_state_mask |= 2;
    } else {
      hg_state |= 0x20;
    }
  }

  if((val & 4)) {
    if(hg_state & 0x40) {
      hg_state_mask |= 4;
    } else {
      hg_state |= 0x40;
    }
  }

  if((val & 8)) {
    if(hg_state & 0x80) {
      hg_state_mask |= 8;
    } else {
      hg_state |= 0x80;
    }
  }  
}

enum fuse_test_display_state {
	FUSE_DISPLAY_WAIT       = 0,
	FUSE_DISPLAY_TESTING_P1,
	FUSE_DISPLAY_TESTING_P2,	
	FUSE_DISPLAY_TESTING_P3,
	FUSE_DISPLAY_TESTING_M,
// fuse test
	FUSE_DISPLAY_FUSE0_SHORT, // 001
	FUSE_DISPLAY_FUSE0_BROKE, // 002
	FUSE_DISPLAY_FUSE0_ERROR,	// 003
	FUSE_DISPLAY_FUSE1_SHORT, // 004
	FUSE_DISPLAY_FUSE1_BROKE, // 005
	FUSE_DISPLAY_FUSE1_ERROR,	// 006
// tripwire 
	FUSE_DISPLAY_TRIPWIRE_BROKE, // 101
	FUSE_DISPLAY_TRIPWIRE_ERROR, // 102
// thermo
	FUSE_DISPLAY_THERMO_TOO_HI, // 201
	FUSE_DISPLAY_THERMO_TOO_LO, // 202
	FUSE_DISPLAY_THERMO_HI_ERROR,  // 203
	FUSE_DISPLAY_THERMO_LO_ERROR,  // 203
// hg
	FUSE_DISPLAY_HG_ERROR,    // 301
// gyro
	FUSE_DISPLAY_GYRO_ERROR,   // 401
	FUSE_DISPLAY_GOOD 
};

enum fuse_test_phase {
		FUSE_TEST_PHASE_FUSE0_SHORT = 0,
		FUSE_TEST_PHASE_FUSE0_BROKE,	
		FUSE_TEST_PHASE_FUSE1_SHORT,
		FUSE_TEST_PHASE_FUSE1_BROKE,
		FUSE_TEST_PHASE_TRIPWIRE,	
		FUSE_TEST_PHASE_THERMO_HI,	
		FUSE_TEST_PHASE_THERMO_LO,
		FUSE_TEST_PHASE_HG,
		FUSE_TEST_PHASE_GYRO	
};

static void display_error_code(enum fuse_test_phase phase, int err)
{
  CDBG("display_error_code phase = %bd, error = %d\n", phase, err);
	led_clear();
	if(err != 0) {
		led_set_code(5, 'E');
		led_set_code(4, phase + 0x30);
		led_set_code(3, '-');		
		led_set_code(2, (err / 100) + 0x30);
		led_set_code(1, ((err % 100)/10) + 0x30);  
		led_set_code(0, (err % 10) + 0x30); 
	} else {
		led_set_code(5, 'P');
		led_set_code(4, 'A');
		led_set_code(3, 'S');  
		led_set_code(2, 'S');
		led_set_code(1, '-');	
		led_set_code(0, phase + 0x30);
	}
}

static void display_fuse_state(enum fuse_test_phase phase, enum fuse_test_display_state state, char value)
{
	int err = -1;
  
  CDBG("display_fuse_state phase %bd, state %bd value %bd\n", phase, state, value);
  
	led_clear();
	
	switch(state) {
		case FUSE_DISPLAY_WAIT:
			led_set_code(5, 'P');
			led_set_code(4, phase + 0x30);
			led_set_code(3, '0');
			led_set_blink(3);
			break;
		case FUSE_DISPLAY_TESTING_P1:
			led_set_code(5, 'P');
			led_set_code(4, phase + 0x30);
			led_set_code(3, '1');
			led_set_blink(3);
			break;
		case FUSE_DISPLAY_TESTING_P2:
			led_set_code(5, 'P');
			led_set_code(4, phase + 0x30);
			led_set_code(3, '2');
			led_set_blink(3);
      if(phase == FUSE_TEST_PHASE_THERMO_HI || phase == FUSE_TEST_PHASE_THERMO_LO) {
        if(value < 0) {
          led_set_code(2, '-');
          value = 0 - value;
        }
        led_set_code(1, value / 10 + 0x30);
        led_set_code(0, value % 10 + 0x30);        
      }
			break;
		case FUSE_DISPLAY_TESTING_P3:
			led_set_code(5, 'P');
			led_set_code(4, phase + 0x30);
			led_set_code(3, '3');
			led_set_blink(3);
			break;
		case FUSE_DISPLAY_TESTING_M:
			led_set_code(5, 'P');
			led_set_code(4, phase + 0x30);
       CDBG("FUSE_DISPLAY_TESTING_M value = %bx\n", value);
			led_set_code(3, (value & 0x8) != 0 ? '1' : '0');
			led_set_code(2, (value & 0x4) != 0 ? '1' : '0');
			led_set_code(1, (value & 0x2) != 0 ? '1' : '0');
			led_set_code(0, (value & 0x1) != 0 ? '1' : '0');
			break;
// fuse test
		case FUSE_DISPLAY_FUSE0_SHORT: // 001
			err = 1; 
			break;		
		case FUSE_DISPLAY_FUSE0_BROKE: // 002
			err = 2; 
			break;
		case FUSE_DISPLAY_FUSE0_ERROR:	// 003
			err = 3; 
			break;
		case FUSE_DISPLAY_FUSE1_SHORT: // 004
			err = 4; 
			break;
		case FUSE_DISPLAY_FUSE1_BROKE: // 005
			err = 5; 
			break;
		case FUSE_DISPLAY_FUSE1_ERROR:	// 006
			err = 6; 
			break;
// tripwire 
		case FUSE_DISPLAY_TRIPWIRE_BROKE: // 101
			err = 101; 
			break;
		case FUSE_DISPLAY_TRIPWIRE_ERROR: // 102
			err = 102; 
			break;
// thermo
		case FUSE_DISPLAY_THERMO_TOO_HI: // 201
			err = 201; 
			break;
		case FUSE_DISPLAY_THERMO_TOO_LO: // 202
			err = 202; 
			break;
		case FUSE_DISPLAY_THERMO_HI_ERROR:  // 203
			err = 203; 
			break;
		case FUSE_DISPLAY_THERMO_LO_ERROR:  // 204
			err = 204; 
			break;
// hg
		case FUSE_DISPLAY_HG_ERROR:    // 301
			err = 301; 
			break;
// gyro
		case FUSE_DISPLAY_GYRO_ERROR:   // 401
			err = 401; 
			break;
		case FUSE_DISPLAY_GOOD:
			err = 0; 
			break;
}

	if(err != -1) {
		display_error_code(phase, err);
	}

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
  
	if(get_sm_ss_state(from) == SM_FUSE_TEST_INIT 
		&& get_sm_ss_state(to) == SM_FUSE_TEST_INIT
		&& ev == EV_1S) {
			in_testing ++;
			if(in_testing >= 2) {
				set_task(EV_FUSE_SEL0);
				in_testing = 0;
			}
		}
	
  // 从别的状态切换过来，防止误操作
  if(get_sm_ss_state(from) == SM_FUSE_TEST_INIT
    && get_sm_ss_state(to) == SM_FUSE_TEST_FUSE0_SHORT
    && ev == EV_FUSE_SEL0 ) {
			display_fuse_state(FUSE_TEST_PHASE_FUSE0_SHORT, FUSE_DISPLAY_WAIT, 0);
			CDBG("fuse0 short begin test\n");
    return;
  }
	
	// mod0 切换测试
	if(get_sm_ss_state(to) == SM_FUSE_TEST_FUSE0_BROKE && ev == EV_KEY_MOD_PRESS && in_testing == 0) {
		display_fuse_state(FUSE_TEST_PHASE_FUSE0_BROKE, FUSE_DISPLAY_WAIT, 0);
		CDBG("fuse0 broke begin test\n");
		return;
	}
	
	// mod0 切换测试
	if(get_sm_ss_state(to) == SM_FUSE_TEST_FUSE1_SHORT && ev == EV_KEY_MOD_PRESS && in_testing == 0) {
		display_fuse_state(FUSE_TEST_PHASE_FUSE1_SHORT, FUSE_DISPLAY_WAIT, 0);
		CDBG("fuse1 short begin test\n");
		return;
	}
	
	// mod0 切换测试
	if(get_sm_ss_state(to) == SM_FUSE_TEST_FUSE1_BROKE && ev == EV_KEY_MOD_PRESS && in_testing == 0) {
		display_fuse_state(FUSE_TEST_PHASE_FUSE1_BROKE, FUSE_DISPLAY_WAIT, 0);
		CDBG("fuse1 broke begin test\n");
		return;
	}
	
	// set0 启动测试 fuse0 fuse1 short broke
	if((get_sm_ss_state(to) == SM_FUSE_TEST_FUSE0_SHORT 
		|| get_sm_ss_state(to) == SM_FUSE_TEST_FUSE0_BROKE
		|| get_sm_ss_state(to) == SM_FUSE_TEST_FUSE1_SHORT
		|| get_sm_ss_state(to) == SM_FUSE_TEST_FUSE1_BROKE
		)
		&& ev == EV_KEY_SET_PRESS && in_testing == 0) {
		if(get_sm_ss_state(to) == SM_FUSE_TEST_FUSE0_SHORT ) {
			display_fuse_state(FUSE_TEST_PHASE_FUSE0_SHORT, FUSE_DISPLAY_TESTING_P1, 0);
			CDBG("fuse0 short into P1\n");
		} else if(get_sm_ss_state(to) == SM_FUSE_TEST_FUSE0_BROKE ) {
			display_fuse_state(FUSE_TEST_PHASE_FUSE0_BROKE, FUSE_DISPLAY_TESTING_P1, 0);
			CDBG("fuse0 broke into P1\n");
		} else if(get_sm_ss_state(to) == SM_FUSE_TEST_FUSE1_SHORT ) {
			display_fuse_state(FUSE_TEST_PHASE_FUSE1_SHORT, FUSE_DISPLAY_TESTING_P1, 0);
			CDBG("fuse1 short into P1\n");
		} else  {
			display_fuse_state(FUSE_TEST_PHASE_FUSE1_BROKE, FUSE_DISPLAY_TESTING_P1, 0);
			CDBG("fuse1 broke into P1\n");
		}
		fuse_enable(1);
		in_testing = 1;
		begin_test_sec = clock_get_sec_256();
		return;
	}
	
	// 测试状态 fuse0 fuse1 short broke 
	if(  (get_sm_ss_state(to) == SM_FUSE_TEST_FUSE0_SHORT && (ev == EV_1S || ev == EV_FUSE0_SHORT))
		|| (get_sm_ss_state(to) == SM_FUSE_TEST_FUSE0_BROKE && (ev == EV_1S || ev == EV_FUSE0_BROKE))
		|| (get_sm_ss_state(to) == SM_FUSE_TEST_FUSE1_SHORT && (ev == EV_1S || ev == EV_FUSE1_SHORT))
		|| (get_sm_ss_state(to) == SM_FUSE_TEST_FUSE1_BROKE && (ev == EV_1S || ev == EV_FUSE1_BROKE))
		) {
		if(in_testing == 1 && ev == EV_1S) { // so far so good, continue!
			if(time_diff_now(begin_test_sec) > FUSE_TEST_TIMEO) {
				if(get_sm_ss_state(to) == SM_FUSE_TEST_FUSE0_SHORT ) {
					CDBG("fuse0 P1 short good\n");
					fuse_set_fuse_short(0, 1);
					display_fuse_state(FUSE_TEST_PHASE_FUSE0_SHORT, FUSE_DISPLAY_TESTING_P2, 0);
				} else if (get_sm_ss_state(to) == SM_FUSE_TEST_FUSE0_BROKE ) {
					CDBG("fuse0 P1 broke good\n");
					fuse_set_fuse_broke(0, 1);
					display_fuse_state(FUSE_TEST_PHASE_FUSE0_BROKE, FUSE_DISPLAY_TESTING_P2, 0);
				} else if (get_sm_ss_state(to) == SM_FUSE_TEST_FUSE1_SHORT ) {
					CDBG("fuse1 P1 short good\n");
					fuse_set_fuse_short(1, 1);
					display_fuse_state(FUSE_TEST_PHASE_FUSE1_SHORT, FUSE_DISPLAY_TESTING_P2, 0);
				} else {
					CDBG("fuse1 P1 broke good\n");
					fuse_set_fuse_broke(1, 1);
					display_fuse_state(FUSE_TEST_PHASE_FUSE1_BROKE, FUSE_DISPLAY_TESTING_P2, 0);
				}
				in_testing = 2;
				begin_test_sec = clock_get_sec_256();
			}
		} else if(in_testing == 1 
			&& (ev == EV_FUSE0_SHORT
				|| ev == EV_FUSE0_BROKE
				|| ev == EV_FUSE1_SHORT
				|| ev == EV_FUSE1_BROKE)) { // bad: is short or broke
			if(ev == EV_FUSE0_SHORT) {
				display_fuse_state(FUSE_TEST_PHASE_FUSE0_SHORT, FUSE_DISPLAY_FUSE0_SHORT, 0);
				CDBG("fuse0 P1 failed: already short\n");
			} else if(ev == EV_FUSE0_BROKE) {
				display_fuse_state(FUSE_TEST_PHASE_FUSE0_BROKE, FUSE_DISPLAY_FUSE0_BROKE, 0);
				CDBG("fuse0 P1 failed: already broke\n");
			} else if (ev == EV_FUSE1_SHORT) {
				display_fuse_state(FUSE_TEST_PHASE_FUSE1_SHORT, FUSE_DISPLAY_FUSE1_SHORT, 0);
				CDBG("fuse1 P1 failed: already short\n");
			} else {
				display_fuse_state(FUSE_TEST_PHASE_FUSE1_BROKE, FUSE_DISPLAY_FUSE1_BROKE, 0);
				CDBG("fuse0 P1 failed: already broke\n");
			}
			in_testing = 0;
			fuse_enable(0);
		}else if(in_testing == 2 && ev == EV_1S) { // bad, 不响应
			if(time_diff_now(begin_test_sec) > FUSE_TEST_TIMEO) {
				if(get_sm_ss_state(to) == SM_FUSE_TEST_FUSE0_SHORT ) {
					CDBG("fuse0 P2 failed: short not response\n");
					fuse_set_fuse_short(0, 0);
					display_fuse_state(FUSE_TEST_PHASE_FUSE0_SHORT, FUSE_DISPLAY_FUSE0_ERROR, 0);
				} else if(get_sm_ss_state(to) == SM_FUSE_TEST_FUSE0_BROKE ) {
					CDBG("fuse0 P2 failed: broke not response\n");
					fuse_set_fuse_broke(0, 0);
					display_fuse_state(FUSE_TEST_PHASE_FUSE0_BROKE, FUSE_DISPLAY_FUSE0_ERROR, 0);
				} else if(get_sm_ss_state(to) == SM_FUSE_TEST_FUSE1_SHORT ) {
					CDBG("fuse1 P2 failed: short not response\n");
					fuse_set_fuse_short(1, 0);
					display_fuse_state(FUSE_TEST_PHASE_FUSE0_SHORT, FUSE_DISPLAY_FUSE0_ERROR, 0);
				} else {
					CDBG("fuse1 P2 failed: broke not response\n");
					fuse_set_fuse_broke(1, 0);
					display_fuse_state(FUSE_TEST_PHASE_FUSE0_BROKE, FUSE_DISPLAY_FUSE0_ERROR, 0);
				}
				in_testing = 0;
				fuse_enable(0);
			}
		} else if(in_testing == 2 
		&&  (ev == EV_FUSE0_SHORT
				|| ev == EV_FUSE0_BROKE
				|| ev == EV_FUSE1_SHORT
				|| ev == EV_FUSE1_BROKE)) { // good!
			if(ev == EV_FUSE0_SHORT) {
				CDBG("fuse0 P2 short good\n");
				fuse_set_fuse_short(0, 0);
				display_fuse_state(FUSE_TEST_PHASE_FUSE0_SHORT, FUSE_DISPLAY_GOOD, 0);
			} else if(ev == EV_FUSE0_BROKE) {
				CDBG("fuse0 P2 broke good\n");					
				fuse_set_fuse_broke(0, 0);
				display_fuse_state(FUSE_TEST_PHASE_FUSE0_BROKE, FUSE_DISPLAY_GOOD, 0);
			} else if(ev == EV_FUSE1_SHORT) {
				CDBG("fuse1 P2 short good\n");			
				fuse_set_fuse_short(1, 0);
				display_fuse_state(FUSE_TEST_PHASE_FUSE1_SHORT, FUSE_DISPLAY_GOOD, 0);	
			} else {
				CDBG("fuse1 P2 broke good\n");
				fuse_set_fuse_broke(1, 0);
				display_fuse_state(FUSE_TEST_PHASE_FUSE1_BROKE, FUSE_DISPLAY_GOOD, 0);			
			}
			in_testing = 0;
			fuse_enable(0);
		}
	}
    
  // thermo测试
	// mod0 切换测试
	if((get_sm_ss_state(to) == SM_FUSE_TEST_THERMO_HI
    || get_sm_ss_state(to) == SM_FUSE_TEST_THERMO_LO) && ev == EV_KEY_MOD_PRESS && in_testing == 0) {
    if(get_sm_ss_state(to) == SM_FUSE_TEST_THERMO_HI) {
      display_fuse_state(FUSE_TEST_PHASE_THERMO_HI, FUSE_DISPLAY_WAIT, 0);
    } else {
      display_fuse_state(FUSE_TEST_PHASE_THERMO_LO, FUSE_DISPLAY_WAIT, 0);
    }
		return;
	}
    
  // set0 开始测试
	if((get_sm_ss_state(to) == SM_FUSE_TEST_THERMO_HI 
    || get_sm_ss_state(to) == SM_FUSE_TEST_THERMO_LO) && ev == EV_KEY_SET_PRESS && in_testing == 0) {
    in_testing = 1;
    begin_test_sec = clock_get_sec_256();
    if(get_sm_ss_state(to) == SM_FUSE_TEST_THERMO_HI) {
      thermo_hi_enable(1);
      display_fuse_state(FUSE_TEST_PHASE_THERMO_HI, FUSE_DISPLAY_TESTING_P1, 0);
    } else {
      thermo_lo_enable(1);
      display_fuse_state(FUSE_TEST_PHASE_THERMO_LO, FUSE_DISPLAY_TESTING_P1, 0);
    }
		return;
	}
  
	if((get_sm_ss_state(to) == SM_FUSE_TEST_THERMO_HI
    || get_sm_ss_state(to) == SM_FUSE_TEST_THERMO_LO) && ev == EV_1S) {
	  // P1 OK, into P2
		if(in_testing == 1 && time_diff_now(begin_test_sec) > FUSE_TEST_TIMEO) {
			in_testing = 2;
			begin_test_sec = clock_get_sec_256();
      if(get_sm_ss_state(to) == SM_FUSE_TEST_THERMO_HI) {
        display_fuse_state(FUSE_TEST_PHASE_THERMO_HI, FUSE_DISPLAY_TESTING_P2, 0);
      } else {
        display_fuse_state(FUSE_TEST_PHASE_THERMO_LO, FUSE_DISPLAY_TESTING_P2, 0);
      }
      return;
		// P2 Failed，没响应？
		} else if(in_testing == 2) {
      if(get_sm_ss_state(to) == SM_FUSE_TEST_THERMO_HI && thermo_hi_threshold_reach_bottom()) {
        in_testing = 0;
        thermo_hi_threshold_reset();
        thermo_hi_enable(0);
        display_fuse_state(FUSE_TEST_PHASE_THERMO_HI, FUSE_DISPLAY_THERMO_HI_ERROR, 0);
      } else if(get_sm_ss_state(to) == SM_FUSE_TEST_THERMO_LO && thermo_lo_threshold_reach_top()){
        in_testing = 0;
        thermo_lo_threshold_reset();
        thermo_lo_enable(0);
        display_fuse_state(FUSE_TEST_PHASE_THERMO_LO, FUSE_DISPLAY_THERMO_LO_ERROR, 0);
      } else if(get_sm_ss_state(to) == SM_FUSE_TEST_THERMO_HI) {
        thermo_hi_threshold_dec();
        display_fuse_state(FUSE_TEST_PHASE_THERMO_HI, FUSE_DISPLAY_TESTING_P2, thermo_hi_threshold_get());
      } else {
        thermo_lo_threshold_inc();
        display_fuse_state(FUSE_TEST_PHASE_THERMO_LO, FUSE_DISPLAY_TESTING_P2, thermo_lo_threshold_get());
      }
		}
		return;
	}
    
	if((get_sm_ss_state(to) == SM_FUSE_TEST_THERMO_HI
    ||get_sm_ss_state(to) == SM_FUSE_TEST_THERMO_LO) && (ev == EV_THERMO_HI || ev == EV_THERMO_LO)) {
		if(in_testing == 1) { // P1 Failed， 已经处于温度激活状态？
			in_testing = 0;
      if(get_sm_ss_state(to) == SM_FUSE_TEST_THERMO_HI) {
        thermo_hi_threshold_reset();
        thermo_hi_enable(0);
        display_fuse_state(FUSE_TEST_PHASE_THERMO_HI, FUSE_DISPLAY_THERMO_TOO_HI, 0);
      } else {
        thermo_lo_threshold_reset();
        thermo_lo_enable(0);
        display_fuse_state(FUSE_TEST_PHASE_THERMO_LO, FUSE_DISPLAY_THERMO_TOO_LO, 0);
      }
		} else if(in_testing == 2) { // P2 OK
			in_testing = 0;
      if(get_sm_ss_state(to) == SM_FUSE_TEST_THERMO_HI) {
        thermo_hi_threshold_reset();
        thermo_hi_enable(0);
        display_fuse_state(FUSE_TEST_PHASE_THERMO_HI, FUSE_DISPLAY_GOOD, 0);
      } else {
        thermo_lo_threshold_reset();
        thermo_lo_enable(0);
        display_fuse_state(FUSE_TEST_PHASE_THERMO_LO, FUSE_DISPLAY_GOOD, 0);
      }
		}
		return;
	}
  
	// tripwire 测试
	// mod0 切换测试
	if((get_sm_ss_state(to) == SM_FUSE_TEST_TRIPWIRE) && ev == EV_KEY_MOD_PRESS && in_testing == 0) {
    display_fuse_state(FUSE_TEST_PHASE_TRIPWIRE, FUSE_DISPLAY_WAIT, 0);
		return;
	}
	
	// set0 开始测试
	if((get_sm_ss_state(to) == SM_FUSE_TEST_TRIPWIRE ) && ev == EV_KEY_SET_PRESS && in_testing == 0) {
    in_testing = 1;
    begin_test_sec = clock_get_sec_256();
    tripwire_enable(1);
    display_fuse_state(FUSE_TEST_PHASE_TRIPWIRE, FUSE_DISPLAY_TESTING_P1, 0);
		return;
	}
	
	
	if((get_sm_ss_state(to) == SM_FUSE_TEST_TRIPWIRE)&& ev == EV_1S) {
	  // P1 OK, into P2
		if(in_testing == 1 && time_diff_now(begin_test_sec) > FUSE_TEST_TIMEO) {
			in_testing = 2;
			begin_test_sec = clock_get_sec_256();
      tripwire_set_broke(1);
      display_fuse_state(FUSE_TEST_PHASE_TRIPWIRE, FUSE_DISPLAY_TESTING_P2, 0);
		// P2 Failed，没响应？
		} else if(in_testing == 2 && time_diff_now(begin_test_sec) > FUSE_TEST_TIMEO) {
			in_testing = 0;
      tripwire_set_broke(0);
      tripwire_enable(0);
      display_fuse_state(FUSE_TEST_PHASE_TRIPWIRE, FUSE_DISPLAY_TRIPWIRE_ERROR, 0);
		}
		return;
	}
	
	if((get_sm_ss_state(to) == SM_FUSE_TEST_TRIPWIRE && ev == EV_TRIPWIRE)) {
		if(in_testing == 1) { // P1 Failed， 已经处于断开状态/温度激活状态？
      display_fuse_state(FUSE_TEST_PHASE_TRIPWIRE, FUSE_DISPLAY_TRIPWIRE_BROKE, 0);
		} else if(in_testing == 2) { // P2 OK
      display_fuse_state(FUSE_TEST_PHASE_TRIPWIRE, FUSE_DISPLAY_GOOD, 0);
		}
    in_testing = 0;
    tripwire_set_broke(0);
    tripwire_enable(0);
		return;
	}
	
	// 进入HG测试
	if(get_sm_ss_state(to) == SM_FUSE_TEST_HG && ev == EV_KEY_MOD_PRESS && in_testing == 0) {
		display_fuse_state(FUSE_TEST_PHASE_HG, FUSE_DISPLAY_WAIT, 0);
		return;
	}
	// 启动HG测试
	if(get_sm_ss_state(to) == SM_FUSE_TEST_HG && ev == EV_KEY_SET_PRESS && in_testing == 0) {
		in_testing = 1;
		begin_test_sec = clock_get_sec_256();
		hg_enable(1);
		hg_state_mask = 0;
    hg_state = hg_get_state();
		display_fuse_state(FUSE_TEST_PHASE_HG, FUSE_DISPLAY_TESTING_M, hg_state_mask);
		return;
	}
	// 等待人去转
	if(get_sm_ss_state(to) == SM_FUSE_TEST_HG && ev == EV_ROTATE_HG && in_testing == 1) {
		test_hg_state_mask(hg_get_state());
		display_fuse_state(FUSE_TEST_PHASE_HG, FUSE_DISPLAY_TESTING_M, hg_state_mask);
		if(hg_state_mask  == 0xF) { // OK!
			hg_enable(0);
			display_fuse_state(FUSE_TEST_PHASE_HG, FUSE_DISPLAY_GOOD, 0);
			in_testing = 0;
		}
		return;
	}
	// 检查是否超时
	if(get_sm_ss_state(to) == SM_FUSE_TEST_HG && ev == EV_1S) {
    CDBG(" %bd %bd time_diff = %bd\n", clock_get_sec_256(), begin_test_sec, time_diff_now(begin_test_sec));
		if(in_testing == 1 && time_diff_now(begin_test_sec) > HG_TEST_TIMEO) {
			hg_enable(0);
			display_fuse_state(FUSE_TEST_PHASE_HG, FUSE_DISPLAY_HG_ERROR, 0);
			in_testing = 0;
		}
		return;
	}
	
	
	// 进入Gyro测试
	if(get_sm_ss_state(to) == SM_FUSE_TEST_GYRO && ev == EV_KEY_MOD_PRESS && in_testing == 0) {
		display_fuse_state(FUSE_TEST_PHASE_GYRO, FUSE_DISPLAY_WAIT, 0);
		return;
	}
	// 启动Gyro测试
	if(get_sm_ss_state(to) == SM_FUSE_TEST_GYRO && ev == EV_KEY_SET_PRESS && in_testing == 0) {
		in_testing = 1;
		begin_test_sec = clock_get_sec_256();
		gyro_enable(1);
		display_fuse_state(FUSE_TEST_PHASE_GYRO, FUSE_DISPLAY_TESTING_P1, 0);
		return;
	}
	// 等待人去晃
	if(get_sm_ss_state(to) == SM_FUSE_TEST_GYRO && ev == EV_ACC_GYRO && in_testing == 1) {
    display_fuse_state(FUSE_TEST_PHASE_GYRO, FUSE_DISPLAY_TESTING_P2, 0);
		in_testing = 2;
		begin_test_sec = clock_get_sec_256();
		return;
	}
	// 等待人丢
	if(get_sm_ss_state(to) == SM_FUSE_TEST_GYRO && ev == EV_DROP_GYRO && in_testing == 2) {
		display_fuse_state(FUSE_TEST_PHASE_GYRO, FUSE_DISPLAY_TESTING_P3, 0);
		in_testing = 3;
		begin_test_sec = clock_get_sec_256();
		return;
	}
	// 等待人去转
	if(get_sm_ss_state(to) == SM_FUSE_TEST_GYRO && ev == EV_ROTATE_GYRO && in_testing == 3) {
		display_fuse_state(FUSE_TEST_PHASE_GYRO, FUSE_DISPLAY_GOOD, 0);
		in_testing = 0;
		return;
	}
	// 检查是否超时
	if(get_sm_ss_state(to) == SM_FUSE_TEST_GYRO && ev == EV_1S) {
		if(time_diff_now(begin_test_sec) > GYRO_TEST_TIMEO) {
			gyro_enable(0);
			display_fuse_state(FUSE_TEST_PHASE_GYRO, FUSE_DISPLAY_GYRO_ERROR, 0);
			in_testing = 0;
		}
		return;
	}
}