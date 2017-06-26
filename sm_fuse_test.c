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
#define HG_TEST_TIMEO   10
#define GYRO_TEST_TIMEO 10

static unsigned char in_testing;
static unsigned char begin_test_sec;
static unsigned char hg_state; // ��4λ�仯��¼����4λ��ʼ״̬

static void test_hg_state(unsigned char state)
{
	// bit�Ƚ�state��4λ��hg_state��4λ�� �������ȣ���hg_state��4λ��Ӧ��λ
	hg_state |= ((hg_state & 0xF0) ^ (state & 0xF0)) << 4;
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


static void display_error_code(unsigned char err)
{
	led_clear();
	if(err != 0) {
		led_set_code(5, 'E');
		led_set_code(4, (err / 100) + 0x30);
		led_set_code(3, ((err % 100)/10) + 0x30);  
		led_set_code(2, (err % 10) + 0x30); 
	} else {
		led_set_code(5, 'P');
		led_set_code(4, 'A');
		led_set_code(3, 'S');  
		led_set_code(2, 'S');
	}
}

static void display_fuse_state(enum fuse_test_display_state state, unsigned char value)
{
	char err = -1;
	switch(state) {
		case FUSE_DISPLAY_WAIT:
			led_set_code(5, 'P');
			led_set_code(4, '0');
			led_set_blink(4);
			break;
		case FUSE_DISPLAY_TESTING_P1:
			led_set_code(5, 'P');
			led_set_code(4, '1');
			led_set_blink(4);
			break;
		case FUSE_DISPLAY_TESTING_P2:
			led_set_code(5, 'P');
			led_set_code(4, '2');
			led_set_blink(4);
			break;
		case FUSE_DISPLAY_TESTING_P3:
			led_set_code(5, 'P');
			led_set_code(4, '3');
			led_set_blink(4);
			break;
		case FUSE_DISPLAY_TESTING_M:
			led_set_code(5, 'P');
			led_set_code(4, value & 0x8 != 0 ? '1' : '0');
			led_set_blink(4);
			led_set_code(3, value & 0x4 != 0 ? '1' : '0');
			led_set_blink(3);
			led_set_code(2, value & 0x2 != 0 ? '1' : '0');
			led_set_blink(2);
			led_set_code(1, value & 0x1 != 0 ? '1' : '0');
			led_set_blink(1);
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
		display_error_code(err);
	}

}


void sm_fuse_test(unsigned char from, unsigned char to, enum task_events ev)
{
  CDBG("sm_fuse_test %bd %bd %bd\n", from, to, ev);

  // ��modset1����fuse���ܼ���
  if(get_sm_ss_state(to) == SM_FUSE_TEST_INIT && ev == EV_KEY_MOD_SET_LPRESS) {
    alarm_switch_off();
    lt_timer_switch_on();
    display_logo(DISPLAY_LOGO_TYPE_FUSE, 0);
    return;
  }
  
  // �ӱ��״̬�л���������ֹ�����
  if(get_sm_ss_state(from) == SM_FUSE_TEST_INIT 
    && get_sm_ss_state(to) == SM_FUSE_TEST_FUSE0_SHORT
    && (ev == EV_KEY_MOD_UP || ev == EV_KEY_SET_UP)) {
			display_fuse_state(0, FUSE_DISPLAY_WAIT);
    return;
  }
	
	// mod0 �л�����
	if(get_sm_ss_state(to) == SM_FUSE_TEST_FUSE0_BROKE && ev == EV_KEY_MOD_PRESS && in_testing == 0) {
		display_fuse_state(FUSE_DISPLAY_WAIT, 0);
		return;
	}
	
	// mod0 �л�����
	if(get_sm_ss_state(to) == SM_FUSE_TEST_FUSE1_SHORT && ev == EV_KEY_MOD_PRESS && in_testing == 0) {
		display_fuse_state(FUSE_DISPLAY_WAIT, 0);
		return;
	}
	
	// mod0 �л�����
	if(get_sm_ss_state(to) == SM_FUSE_TEST_FUSE1_BROKE && ev == EV_KEY_MOD_PRESS && in_testing == 0) {
		display_fuse_state(FUSE_DISPLAY_WAIT, 0);
		return;
	}
	
	// set0 �������� fuse0 fuse1 short broke
	if((get_sm_ss_state(to) == SM_FUSE_TEST_FUSE0_SHORT 
		|| get_sm_ss_state(to) == SM_FUSE_TEST_FUSE0_BROKE
		|| get_sm_ss_state(to) == SM_FUSE_TEST_FUSE1_SHORT
		|| get_sm_ss_state(to) == SM_FUSE_TEST_FUSE1_BROKE
		)
		&& ev == EV_KEY_SET_PRESS && in_testing == 0) {
		fuse_enable(1);
		in_testing = 1;
		begin_test_sec = clock_get_sec();
		/*
		if(get_sm_ss_state(to) == SM_FUSE_TEST_FUSE0_SHORT 
			|| get_sm_ss_state(to) == SM_FUSE_TEST_FUSE0_BROKE )
			display_fuse_state(0, FUSE_DISPLAY_TESTING_P1);
		else {
			display_fuse_state(1, FUSE_DISPLAY_TESTING_P1);
		}
		*/
		display_fuse_state(FUSE_DISPLAY_TESTING_P1, 0);
		return;
	}
	
	// ����״̬ fuse0 fuse1 short broke 
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
				display_fuse_state(FUSE_DISPLAY_TESTING_P2, 0);
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
				display_fuse_state(FUSE_DISPLAY_FUSE0_SHORT, 0);
			else if(ev == EV_FUSE0_BROKE)
				display_fuse_state(FUSE_DISPLAY_FUSE0_BROKE, 0);
			else if (ev == EV_FUSE1_SHORT)
				display_fuse_state(FUSE_DISPLAY_FUSE1_SHORT, 0);
			else
				display_fuse_state(FUSE_DISPLAY_FUSE1_BROKE, 0);
		}else if(in_testing == 2 && ev == EV_1S) { // bad, ����Ӧ
			if(time_diff(clock_get_sec(), begin_test_sec) > FUSE_TEST_TIMEO) {
				in_testing = 0;
				fuse_enable(0);
				if(get_sm_ss_state(to) == SM_FUSE_TEST_FUSE0_SHORT 
					|| get_sm_ss_state(to) == SM_FUSE_TEST_FUSE0_BROKE) {
					fuse_set_fuse_short(0, 0);
					display_fuse_state(FUSE_DISPLAY_FUSE0_ERROR, 0);
				} else{
					fuse_set_fuse_short(1, 0);
					display_fuse_state(FUSE_DISPLAY_FUSE1_ERROR, 0);
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
			} else if(ev == EV_FUSE0_BROKE) {
				fuse_set_fuse_broke(0, 0);
			} else if(ev == EV_FUSE1_SHORT) {
				fuse_set_fuse_short(1, 0);
			} else {
				fuse_set_fuse_broke(1, 0);
			}
			display_fuse_state(FUSE_DISPLAY_GOOD, 0);	
		}
	}
	
	// tripwire ��thermo hi�� thermo lo����
	// mod0 �л�����
	if((get_sm_ss_state(to) == SM_FUSE_TEST_TRIPWIRE 
		|| get_sm_ss_state(to) == SM_FUSE_TEST_THERMO_HI
		|| get_sm_ss_state(to) == SM_FUSE_TEST_THERMO_LO
		)
		&& ev == EV_KEY_MOD_PRESS && in_testing == 0) {
		display_fuse_state(FUSE_DISPLAY_WAIT, 0);
		return;
	}
	
	// set0 ��ʼ����
	if((get_sm_ss_state(to) == SM_FUSE_TEST_TRIPWIRE 
		|| get_sm_ss_state(to) == SM_FUSE_TEST_THERMO_HI
		|| get_sm_ss_state(to) == SM_FUSE_TEST_THERMO_LO
		)
		&& ev == EV_KEY_SET_PRESS && in_testing == 0) {
		in_testing = 1;
		begin_test_sec = clock_get_sec();
		if(get_sm_ss_state(to) == SM_FUSE_TEST_TRIPWIRE) {
			tripwire_enable(1);
		} else if(get_sm_ss_state(to) == SM_FUSE_TEST_THERMO_HI) {
			thermo_hi_enable(1);
		} else {
			thermo_lo_enable(1);
		}
		display_fuse_state(FUSE_DISPLAY_TESTING_P1, 0);
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
			} else if(get_sm_ss_state(to) == SM_FUSE_TEST_THERMO_HI) {
				thermo_hi_set_hit(1);
			} else {
				thermo_lo_set_hit(1);
			}
			display_fuse_state(FUSE_DISPLAY_TESTING_P2, 0);
		// P2 Failed��û��Ӧ��
		} else if(in_testing == 2 && time_diff(clock_get_sec(), begin_test_sec) > FUSE_TEST_TIMEO) {
			in_testing = 0;
			if(get_sm_ss_state(to) == SM_FUSE_TEST_TRIPWIRE, 0) {
				tripwire_enable(0);
				display_fuse_state(FUSE_DISPLAY_TRIPWIRE_ERROR, 0);
			} else if(get_sm_ss_state(to) == SM_FUSE_TEST_THERMO_HI) {
				thermo_hi_enable(0);
				display_fuse_state(FUSE_DISPLAY_THERMO_HI_ERROR, 0);
			} else {
				thermo_lo_enable(0);
				display_fuse_state(FUSE_DISPLAY_THERMO_LO_ERROR, 0);
			}
		}
		return;
	}
	
	if((get_sm_ss_state(to) == SM_FUSE_TEST_TRIPWIRE && ev == EV_TRIPWIRE)
		||(get_sm_ss_state(to) == SM_FUSE_TEST_THERMO_HI && ev == EV_THERMO_HI)
		||(get_sm_ss_state(to) == SM_FUSE_TEST_THERMO_LO && ev == EV_THERMO_LO)
		) {
		if(in_testing == 1) { // P1 Failed�� �Ѿ����ڶϿ�״̬/�¶ȼ���״̬��
			in_testing = 0;
			if(ev == EV_TRIPWIRE) {
				tripwire_enable(0);
				display_fuse_state(FUSE_DISPLAY_TRIPWIRE_BROKE, 0);
			} else if(ev == EV_THERMO_HI){
				thermo_hi_enable(0);
				display_fuse_state(FUSE_DISPLAY_THERMO_TOO_HI, 0);
			} else {
				thermo_lo_enable(0);
				display_fuse_state(FUSE_DISPLAY_THERMO_TOO_LO, 0);
			}
		} else if(in_testing == 2) { // P2 OK
			in_testing = 0;
			if(ev == EV_TRIPWIRE) {
				tripwire_enable(0);
			} else if(ev == EV_THERMO_HI){
				thermo_hi_enable(0);
			} else {
				thermo_lo_enable(0);
			}
			display_fuse_state(FUSE_DISPLAY_GOOD, 0);
		}
		return;
	}
	
	// ����HG����
	if(get_sm_ss_state(to) == SM_FUSE_TEST_HG && ev == EV_KEY_MOD_PRESS && in_testing == 0) {
		display_fuse_state(FUSE_DISPLAY_WAIT, 0);
		return;
	}
	// ����HG����
	if(get_sm_ss_state(to) == SM_FUSE_TEST_HG && ev == EV_KEY_SET_PRESS && in_testing == 0) {
		in_testing = 1;
		begin_test_sec = clock_get_sec();
		hg_enable(1);
		hg_state = hg_get_state();
		display_fuse_state(FUSE_DISPLAY_TESTING_M, hg_state);
		return;
	}
	// �ȴ���ȥת
	if(get_sm_ss_state(to) == SM_FUSE_TEST_HG && ev == EV_ROTATE_HG && in_testing == 1) {
		test_hg_state(hg_get_state());
		display_fuse_state(FUSE_DISPLAY_TESTING_M, hg_state);
		if((hg_state & 0xF0) == 0xF0) { // OK!
			hg_enable(0);
			display_fuse_state(FUSE_DISPLAY_GOOD, 0);
			in_testing = 0;
		}
		return;
	}
	// ����Ƿ�ʱ
	if(get_sm_ss_state(to) == SM_FUSE_TEST_HG && ev == EV_1S) {
		if(in_testing == 1 && time_diff(clock_get_sec(), begin_test_sec) > HG_TEST_TIMEO) {
			hg_enable(0);
			display_fuse_state(FUSE_DISPLAY_HG_ERROR, 0);
			in_testing = 0;
		}
		return;
	}
	
	
	// ����Gyro����
	if(get_sm_ss_state(to) == SM_FUSE_TEST_GYRO && ev == EV_KEY_MOD_PRESS && in_testing == 0) {
		display_fuse_state(FUSE_DISPLAY_WAIT, 0);
		return;
	}
	// ����Gyro����
	if(get_sm_ss_state(to) == SM_FUSE_TEST_GYRO && ev == EV_KEY_SET_PRESS && in_testing == 0) {
		in_testing = 1;
		begin_test_sec = clock_get_sec();
		gyro_enable(1);
		display_fuse_state(FUSE_DISPLAY_TESTING_P1, 0);
		return;
	}
	// �ȴ���ȥ��
	if(get_sm_ss_state(to) == SM_FUSE_TEST_GYRO && ev == EV_ACC_GYRO && in_testing == 1) {
		if(gyro_test_acc_event()) { // OK!
			display_fuse_state(FUSE_DISPLAY_TESTING_P2, 0);
			in_testing = 2;
			begin_test_sec = clock_get_sec();
		}
		return;
	}
	// �ȴ��˶�
	if(get_sm_ss_state(to) == SM_FUSE_TEST_GYRO && ev == EV_ACC_GYRO && in_testing == 2) {
		if(gyro_test_drop_event()) { // OK!
			display_fuse_state(FUSE_DISPLAY_TESTING_P3, 0);
			in_testing = 3;
			begin_test_sec = clock_get_sec();
		}
		return;
	}
	// �ȴ���ȥת
	if(get_sm_ss_state(to) == SM_FUSE_TEST_GYRO && ev == EV_ROTATE_GYRO && in_testing == 3) {
		if(gyro_test_rotate_event()) { // OK!
			display_fuse_state(FUSE_DISPLAY_GOOD, 0);
			in_testing = 0;
		}
		return;
	}
	// ����Ƿ�ʱ
	if(get_sm_ss_state(to) == SM_FUSE_TEST_GYRO && ev == EV_1S) {
		if(time_diff(clock_get_sec(), begin_test_sec) > GYRO_TEST_TIMEO) {
			gyro_enable(0);
			display_fuse_state(FUSE_DISPLAY_GYRO_ERROR, 0);
			in_testing = 0;
		}
		return;
	}
}