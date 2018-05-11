#include "sm_clock_mod_alarm.h"
#include "mod_common.h"
#include "clock.h"
#include "alarm.h"
#include "led.h"
#include "debug.h"
#include "rom.h"
#include "beeper.h"

const char * code sm_clock_mod_alarm_ss_name[] = 
{
  "SM_CLOCK_MODIFY_ALARM_INIT",
  "SM_CLOCK_MODIFY_ALARM_HH",
  "SM_CLOCK_MODIFY_ALARM_MM",
  "SM_CLOCK_MODIFY_ALARM_DAY",
	"SM_CLOCK_MODIFY_ALARM_BS",
	"SM_CLOCK_MODIFY_ALARM_MUSIC",
  NULL
};

#define alarm_index common_state

static void update_alarm(unsigned char what, unsigned char day)
{
  unsigned char hour, min; 
  if(what == IS_HOUR || what == IS_MIN) {
		led_set_code(5, 'A');
		led_set_code(4, 'L'); 
    hour = alarm0_get_hour();
    min  = alarm0_get_min();
    led_set_dp(1);
    led_set_dp(2);
    
    // 如果是12小时显示，以第三位数字的点表示“PM”
    if(alarm0_get_hour_12() && hour > 12) {
      led_set_dp(3);
      hour -= 12;
    } else {
      led_clr_dp(3);
    }
 
    CDBG("update_alarm %bd:%bd\n", hour, min);
      
    if((hour / 10) != 0) {
      led_set_code(3, (hour / 10) + 0x30);
    } else {
      led_set_code(3, LED_CODE_BLACK);
    }
    led_set_code(2, (hour % 10) + 0x30);
    led_set_code(1, (min / 10)+ 0x30);
    led_set_code(0, (min % 10) + 0x30); 
    
  } else if (what == IS_ONOFF){
		CDBG("update_alarm %s\n", alarm0_test_enable(day)? "ON":"OFF");
		led_set_code(5, 'A');
		led_set_code(4, 'L'); 
    led_set_code(3, day + 0x30);
    if(alarm0_test_enable(day)) {
      led_set_code(2, LED_CODE_BLACK);
      led_set_code(1, 'O');
      led_set_code(0, 'N');
    } else {
      led_set_code(2, 'O');
      led_set_code(1, 'F');
      led_set_code(0, 'F');
    }
  } else if(what == IS_BS) {
      led_set_code(5, 'B');
      led_set_code(4, 'S');
		if(alarm1_test_enable()) {
      led_set_code(2, LED_CODE_BLACK);
      led_set_code(1, 'O');
      led_set_code(0, 'N');
		} else {
      led_set_code(2, 'O');
      led_set_code(1, 'F');
      led_set_code(0, 'F');
		}
	} else { // IS_MUSIC
      led_set_code(5, 'S');
      led_set_code(4, 'O');
      led_set_code(3, 'U');
      led_set_code(2, 'N');
      led_set_code(1, 'D');  
      CDBG("beeper_get_music_index return %bd\n", beeper_get_music_index());
      led_set_code(0, beeper_get_music_index() + 1 + 0x30);
	}
}



static void enter_alarm(unsigned char what, unsigned char day)
{
  led_clear();
  switch(what) {
    case IS_HOUR:
      led_set_blink(3);
      led_set_blink(2);    
      break;
    case IS_MIN:
      led_set_blink(1);
      led_set_blink(0);    
      break;      
    case IS_ONOFF:
      led_set_blink(2);
      led_set_blink(1);
      led_set_blink(0);
      break; 
  }
  update_alarm(what, day);
}


static void write_only(unsigned char what)
{
  alarm0_sync_to_rtc();
  switch(what) {
    case IS_HOUR:
      if(lpress_lock_year_hour == 1) {
        lpress_lock_year_hour = 0;
        led_set_blink(3);
        led_set_blink(2); 
      }
      rom_write(ROM_ALARM0_HOUR, alarm0_get_hour());
      break;
    case IS_MIN:
      if(lpress_lock_month_min == 1) {
        lpress_lock_month_min = 0;
        led_set_blink(1);
        led_set_blink(0); 
      }
      rom_write(ROM_ALARM0_HOUR, alarm0_get_min());
      break;
    case IS_ONOFF:
      rom_write(ROM_ALARM0_DAY_MASK, alarm0_get_day_mask());
      break;
  } 
}

static void toggle_alarm(unsigned char day)
{
  alarm0_set_enable(day, !alarm0_test_enable(day));
  update_alarm(IS_ONOFF, day);
  write_only(IS_ONOFF);
}

static void inc_only(unsigned char what)
{
  switch (what) {
    case IS_HOUR:
      if(!lpress_lock_year_hour) {
        lpress_lock_year_hour = 1;
        led_clr_blink(3);
        led_clr_blink(2);        
      }
      alarm0_inc_hour();
      break;
    case IS_MIN:
      if(!lpress_lock_month_min) {
        lpress_lock_month_min = 1;
        led_clr_blink(1);
        led_clr_blink(0); 
      }
      alarm0_inc_min();
      break;
  }
  update_alarm(what, 0);
}

static void inc_write(unsigned char what)
{
  inc_only(what);
  write_only(what);
}

static void toggle_alarm_bs(void)
{
	alarm1_set_enable(!alarm1_test_enable());
	alarm1_sync_to_rtc();
	rom_write(ROM_ALARM1_ENABLE, alarm1_test_enable() ? 1 : 0);
	update_alarm(IS_BS, 0);
}

static void toggle_alarm_music(void)
{
	beeper_inc_music_index();
	rom_write(ROM_BEEPER_MUSIC_INDEX, beeper_get_music_index());
	update_alarm(IS_MUSIC, 0);
}

void sm_clock_mod_alarm_init(unsigned char from, unsigned char to, enum task_events ev)
{
  CDBG("sm_clock_mod_alarm_init %bd %bd %bd\n", from, to, ev);
	display_logo(DISPLAY_LOGO_TYPE_CLOCK, 2);
}

void sm_clock_mod_alarm_submod0(unsigned char from, unsigned char to, enum task_events ev)
{
  CDBG("sm_clock_mod_alarm_init %bd %bd %bd\n", from, to, ev);
 // 切换到修改闹钟
  if(get_sm_ss_state(from) == SM_CLOCK_MODIFY_ALARM_INIT 
    && ev == EV_KEY_MOD_UP) {
    enter_alarm(IS_HOUR, 0);
    return;
  }
  
  // set0小时++ 
  if(ev == EV_KEY_SET_PRESS) {
    inc_write(IS_HOUR);
    return;
  }
  
  // set1小时持续++
  if(ev == EV_KEY_SET_LPRESS) {
    if((lpress_start % LPRESS_INC_DELAY) == 0) {
      inc_only(IS_HOUR);
    }
    lpress_start++;
    if(lpress_start == LPRESS_INC_OVERFLOW) lpress_start = 0;
    return;
  }
  
  // set抬起停止++，写入rtc
  if(ev == EV_KEY_SET_UP) {
    write_only(IS_HOUR);
    lpress_start = 0;
    return;
  }
}

void sm_clock_mod_alarm_submod1(unsigned char from, unsigned char to, enum task_events ev)
{
  CDBG("sm_clock_mod_alarm_submod1 %bd %bd %bd\n", from, to, ev);
  // mod0进入修改分钟模式
  if(ev == EV_KEY_MOD_PRESS) {
    enter_alarm(IS_MIN, 0);
    return;
  } 
  
  
  // set0分钟++
  if(ev == EV_KEY_SET_PRESS) {
    inc_write(IS_MIN);
    return;
  }
  
  // set1分钟持续++
  if(ev == EV_KEY_SET_LPRESS) {
    if((lpress_start % LPRESS_INC_DELAY) == 0) {
      inc_only(IS_MIN);
    }
    lpress_start++;
    if(lpress_start == LPRESS_INC_OVERFLOW) lpress_start = 0;
    return;
  }
  
  // set抬起停止++，写入rtc 
  if(ev == EV_KEY_SET_UP) {
    write_only(IS_MIN);
    lpress_start = 0;
    return;
  }
}

void sm_clock_mod_alarm_submod2(unsigned char from, unsigned char to, enum task_events ev)
{
  CDBG("sm_clock_mod_alarm_submod2 %bd %bd %bd\n", from, to, ev);
	
	if(get_sm_ss_state(from) == SM_CLOCK_MODIFY_ALARM_MM) {
		alarm_index = 1;
		enter_alarm(IS_ONOFF, alarm_index);
		return;
	}
	
	if(ev == EV_KEY_SET_PRESS) {
		toggle_alarm(alarm_index);
	}
	
	if(ev == EV_KEY_MOD_PRESS) {
		alarm_index ++;
		if(alarm_index <= 7) {
			enter_alarm(IS_ONOFF, alarm_index);
		} else {
			set_task(EV_KEY_V0);
		}
	}
}

void sm_clock_mod_alarm_submod3(unsigned char from, unsigned char to, enum task_events ev)
{
  CDBG("sm_clock_mod_alarm_submod3 %bd %bd %bd\n", from, to, ev);
	
	if(ev == EV_KEY_MOD_PRESS) {
		enter_alarm(IS_BS, 0);
	}
	
	if(ev == EV_KEY_SET_PRESS) {
		toggle_alarm_bs();
	}
}

void sm_clock_mod_alarm_submod4(unsigned char from, unsigned char to, enum task_events ev)
{
  CDBG("sm_clock_mod_alarm_submod4 %bd %bd %bd\n", from, to, ev);
	
	if(ev == EV_KEY_MOD_PRESS) {
		enter_alarm(IS_MUSIC, 0);
	}
	
	if(ev == EV_KEY_SET_PRESS) {
		toggle_alarm_music();
	}
}

/*
void sm_clock_mod_alarm(unsigned char from, unsigned char to, enum task_events ev)
{
  CDBG("sm_clock_mod_alarm %bd %bd %bd\n", from, to, ev);
  
  // 按mod1进入修改闹钟模式
  if(get_sm_ss_state(to) == SM_CLOCK_MODIFY_ALARM_INIT && ev == EV_KEY_MOD_LPRESS) {
    display_logo(DISPLAY_LOGO_TYPE_CLOCK, 2);
    return;
  }  
  
  // 切换到修改闹钟
  if(get_sm_ss_state(from) == SM_CLOCK_MODIFY_ALARM_INIT 
    && get_sm_ss_state(to) == SM_CLOCK_MODIFY_ALARM_HH
    && ev == EV_KEY_MOD_UP) {
    enter_alarm(IS_HOUR, 0);
    return;
  }
  
  // set0小时++ 
  if(get_sm_ss_state(to) == SM_CLOCK_MODIFY_ALARM_HH && ev == EV_KEY_SET_PRESS) {
    inc_write(IS_HOUR);
    return;
  }
  
  // set1小时持续++
  if(get_sm_ss_state(to) == SM_CLOCK_MODIFY_ALARM_HH && ev == EV_KEY_SET_LPRESS) {
    if((lpress_start % LPRESS_INC_DELAY) == 0) {
      inc_only(IS_HOUR);
    }
    lpress_start++;
    if(lpress_start == LPRESS_INC_OVERFLOW) lpress_start = 0;
    return;
  }
  
  // set抬起停止++，写入rtc
  if(get_sm_ss_state(to) == SM_CLOCK_MODIFY_ALARM_HH && ev == EV_KEY_SET_UP) {
    write_only(IS_HOUR);
    lpress_start = 0;
    return;
  }
  
  // mod0进入修改分钟模式
  if(get_sm_ss_state(to) == SM_CLOCK_MODIFY_ALARM_MM && ev == EV_KEY_MOD_PRESS) {
    enter_alarm(IS_MIN, 0);
    return;
  } 
  
  
  // set0分钟++
  if(get_sm_ss_state(to) == SM_CLOCK_MODIFY_ALARM_MM && ev == EV_KEY_SET_PRESS) {
    inc_write(IS_MIN);
    return;
  }
  
  // set1分钟持续++
  if(get_sm_ss_state(to) == SM_CLOCK_MODIFY_ALARM_MM && ev == EV_KEY_SET_LPRESS) {
    if((lpress_start % LPRESS_INC_DELAY) == 0) {
      inc_only(IS_MIN);
    }
    lpress_start++;
    if(lpress_start == LPRESS_INC_OVERFLOW) lpress_start = 0;
    return;
  }
  
  // set抬起停止++，写入rtc 
  if(get_sm_ss_state(to) == SM_CLOCK_MODIFY_ALARM_MM && ev == EV_KEY_SET_UP) {
    write_only(IS_MIN);
    lpress_start = 0;
    return;
  }
  
  
  // mod0进入DAY1打开关闭状态
  if(get_sm_ss_state(to) == SM_CLOCK_MODIFY_ALARM_DAY1 && ev == EV_KEY_MOD_PRESS) {
    enter_alarm(IS_DAY_ONOFF, 1);
    return;
  }
  
  // set0 调整DAY1打开关闭，并写入rtc
  if(get_sm_ss_state(to) == SM_CLOCK_MODIFY_ALARM_DAY1 && ev == EV_KEY_SET_PRESS) {
    toggle_alarm(1);
    return;
  }

  // mod0进入DAY2打开关闭状态
  if(get_sm_ss_state(to) == SM_CLOCK_MODIFY_ALARM_DAY2 && ev == EV_KEY_MOD_PRESS) {
    enter_alarm(IS_DAY_ONOFF, 2);
    return;
  }
  
  // set0 调整DAY2打开关闭，并写入rtc
  if(get_sm_ss_state(to) == SM_CLOCK_MODIFY_ALARM_DAY2 && ev == EV_KEY_SET_PRESS) {
    toggle_alarm(2);
    return;
  }

  // mod0进入DAY3打开关闭状态
  if(get_sm_ss_state(to) == SM_CLOCK_MODIFY_ALARM_DAY3 && ev == EV_KEY_MOD_PRESS) {
    enter_alarm(IS_DAY_ONOFF, 3);
    return;
  }
  
  // set0 调整DAY1打开关闭，并写入rtc
  if(get_sm_ss_state(to) == SM_CLOCK_MODIFY_ALARM_DAY3 && ev == EV_KEY_SET_PRESS) {
    toggle_alarm(3);
    return;
  }

  // mod0进入DAY4打开关闭状态
  if(get_sm_ss_state(to) == SM_CLOCK_MODIFY_ALARM_DAY4 && ev == EV_KEY_MOD_PRESS) {
    enter_alarm(IS_DAY_ONOFF, 4);
    return;
  }
  
  // set0 调整DAY4打开关闭，并写入rtc
  if(get_sm_ss_state(to) == SM_CLOCK_MODIFY_ALARM_DAY4 && ev == EV_KEY_SET_PRESS) {
    toggle_alarm(4);
    return;
  }
  
  // mod0进入DAY5打开关闭状态
  if(get_sm_ss_state(to) == SM_CLOCK_MODIFY_ALARM_DAY5 && ev == EV_KEY_MOD_PRESS) {
    enter_alarm(IS_DAY_ONOFF, 5);
    return;
  }
  
  // set0 调整DAY5打开关闭，并写入rtc
  if(get_sm_ss_state(to) == SM_CLOCK_MODIFY_ALARM_DAY5 && ev == EV_KEY_SET_PRESS) {
    toggle_alarm(5);
    return;
  }

  // mod0进入DAY6打开关闭状态
  if(get_sm_ss_state(to) == SM_CLOCK_MODIFY_ALARM_DAY6 && ev == EV_KEY_MOD_PRESS) {
    enter_alarm(IS_DAY_ONOFF, 6);
    return;
  }
  
  // set0 调整DAY6打开关闭，并写入rtc
  if(get_sm_ss_state(to) == SM_CLOCK_MODIFY_ALARM_DAY6 && ev == EV_KEY_SET_PRESS) {
    toggle_alarm(6);
    return;
  }

  // mod0进入DAY7打开关闭状态
  if(get_sm_ss_state(to) == SM_CLOCK_MODIFY_ALARM_DAY7 && ev == EV_KEY_MOD_PRESS) {
    enter_alarm(IS_DAY_ONOFF, 7);
    return;
  }
  // mod0进入DAY7打开关闭状态
  if(get_sm_ss_state(to) == SM_CLOCK_MODIFY_ALARM_DAY7 && ev == EV_KEY_SET_PRESS) {
    toggle_alarm(7);
    return;
  }

  // mod0进入DAY7打开关闭状态
  if(get_sm_ss_state(to) == SM_CLOCK_MODIFY_ALARM_HH && ev == EV_KEY_MOD_PRESS) {
    enter_alarm(IS_HOUR, 0);
    return;
  }
}
*/