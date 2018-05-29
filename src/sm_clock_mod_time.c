#include "sm_clock_mod_time.h"
#include "led.h"
#include "rtc.h"
#include "clock.h"
#include "debug.h"

#include "mod_common.h"

const char * code sm_clock_mod_time_name[] =
{
  "SM_CLOCK_MODIFY_TIME_INIT",
  "SM_CLOCK_MODIFY_TIME_HH",
  "SM_CLOCK_MODIFY_TIME_MM",
  "SM_CLOCK_MODIFY_TIME_SS",
  "SM_CLOCK_MODIFY_TIME_YY",
  "SM_CLOCK_MODIFY_TIME_MO",
  "SM_CLOCK_MODIFY_TIME_DD",
  NULL
};

static void update_hhmmss(void)
{
  unsigned char hour, min, sec;
	
  hour = clock_get_hour();
  min  = clock_get_min();
  sec  = clock_get_sec();
  
  // 两个“:”号
  led_set_dp(1);
  led_set_dp(2);
  led_set_dp(3);
  led_set_dp(4); 
  
  // 如果是12小时显示，以第一位数字的点表示“PM”
  if(clock_get_hour_12() && hour > 12) {
    led_set_dp(5);
    hour -= 12;
  } else if(clock_get_hour_12() && hour == 12) {
		led_set_dp(5);
	} else {
    led_clr_dp(5);
  }
  
  
  CDBG("update_hhmmss %bd:%bd:%bd\n", hour, min, sec);  
  
  if((hour / 10) != 0) {
    led_set_code(5, (hour / 10) + 0x30);
  } else {
    led_set_code(5, LED_CODE_BLACK);
  }
  led_set_code(4, (hour % 10) + 0x30);
  led_set_code(3, (min / 10)  + 0x30);
  led_set_code(2, (min % 10) + 0x30);
  led_set_code(1, (sec / 10) + 0x30);
  led_set_code(0, (sec % 10) + 0x30);
}

static void update_yymmdd(void)
{
  unsigned char year, mon, date;
  
  
  year = clock_get_year();
  mon  = clock_get_month();
  date  = clock_get_date();
  
  led_set_dp(2);
  led_set_dp(4);

  CDBG("update_yymmdd %bd-%bd-%bd\n", year, mon, date);
  
  if((year / 10) != 0) {
    led_set_code(5, (year / 10) + 0x30);
  } else {
    led_set_code(5, LED_CODE_BLACK);
  }
  led_set_code(4, (year % 10) + 0x30);
  led_set_code(3, (mon / 10)+ 0x30);
  led_set_code(2, (mon % 10) + 0x30);
  led_set_code(1, (date / 10) + 0x30);
  led_set_code(0, (date % 10) + 0x30);  
}


static void inc_only(unsigned char what)
{
  switch (what) {
    case IS_HOUR:
    case IS_YEAR:
      if(!lpress_lock_year_hour) {
        lpress_lock_year_hour = 1;
        led_clr_blink(5);
        led_clr_blink(4); 
      }
      if(what == IS_HOUR) {
        clock_inc_hour();
      } else {
        clock_inc_year();
      }
      break;
    case IS_MIN:
    case IS_MON:
      if(!lpress_lock_month_min) {
        lpress_lock_month_min = 1;
        led_clr_blink(3);
        led_clr_blink(2); 
      }
      if(what == IS_MIN) {
        clock_inc_min();
      } else {
        clock_inc_month();
      }
      break;
    case IS_SEC:
    case IS_DAY:
      if(!lpress_lock_day_sec) {
        lpress_lock_day_sec = 1;
        led_clr_blink(1);
        led_clr_blink(0); 
      }
      if(what == IS_SEC) {
        clock_clr_sec();
      } else {
        clock_inc_date();
      }
      break;
  }
	
	if(what == IS_HOUR 
		|| what == IS_MIN 
		|| what == IS_SEC) {
		update_hhmmss();
	} else if(what == IS_YEAR 
		|| what == IS_MON 
		|| what == IS_DAY) {
		update_yymmdd();
	}
}

static void write_only(unsigned char what)
{
  switch(what) {
    case IS_HOUR:
      if(lpress_lock_year_hour == 1) {
        clock_sync_to_rtc(CLOCK_SYNC_TIME);
        lpress_lock_year_hour = 0;
        led_set_blink(5);
        led_set_blink(4);
      }
      break;
    case IS_YEAR:
      if(lpress_lock_year_hour == 1) {
        clock_sync_to_rtc(CLOCK_SYNC_DATE);
        lpress_lock_year_hour = 0;
        led_set_blink(5);
        led_set_blink(4);
      }
      break;
    case IS_MIN:
      if(lpress_lock_month_min == 1) {
        clock_sync_to_rtc(CLOCK_SYNC_TIME);
        lpress_lock_month_min = 0;
        led_set_blink(3);
        led_set_blink(2); 
      }
      break;
    case IS_MON:
      if(lpress_lock_month_min == 1) {
        clock_sync_to_rtc(CLOCK_SYNC_DATE);
        lpress_lock_month_min = 0;
        led_set_blink(3);
        led_set_blink(2);      
      }
      break;  
    case IS_SEC:
      if(lpress_lock_day_sec == 1) {
        clock_sync_to_rtc(CLOCK_SYNC_TIME);
        lpress_lock_day_sec = 0;
        led_set_blink(1);
        led_set_blink(0); 
      }
      break;
    case IS_DAY:
      if(lpress_lock_day_sec == 1) {
        clock_sync_to_rtc(CLOCK_SYNC_DATE);
        lpress_lock_day_sec = 0;
        led_set_blink(1);
        led_set_blink(0); 
      }
      break;  
  }
  
}

static void inc_and_write(unsigned char what)
{
  inc_only(what);
  write_only(what);
}


static void enter_hhmmss(unsigned char what) // blink hour:0, min:1, sec:2
{
  led_clear();
  switch(what) {
    case IS_HOUR:
      led_set_blink(5);
      led_set_blink(4);    
      break;
    case IS_MIN:
      led_set_blink(3);
      led_set_blink(2);    
      break;      
    case IS_SEC:
      led_set_blink(1);
      led_set_blink(0);    
      break;      
  }
  update_hhmmss();
}


static void enter_yymmdd(unsigned char what) // blink year:0, month:1, day:2
{
  led_clear();
  switch(what) {
    case IS_YEAR:
      led_set_blink(5);
      led_set_blink(4);    
      break;
    case IS_MON:
      led_set_blink(3);
      led_set_blink(2);    
      break;      
    case IS_DAY:
      led_set_blink(1);
      led_set_blink(0);    
      break;      
  }
  update_yymmdd();  
}

void sm_clock_mod_time_init(unsigned char from, unsigned char to, enum task_events ev)
{
  CDBG("sm_clock_mod_time_init %bd %bd %bd\n", from, to, ev);
  clock_display(0);
  display_logo(DISPLAY_LOGO_TYPE_CLOCK, 1);
}

static void sm_clock_mod_time(unsigned char what, enum task_events ev)
{
  // 按mod0进入新模式
  if(ev == EV_KEY_MOD_PRESS) {
		if(what == IS_HOUR 
			|| what == IS_MIN 
			|| what == IS_SEC) {
			enter_hhmmss(what);
		} else if(what == IS_YEAR 
			|| what == IS_MON 
			|| what == IS_DAY) {
			enter_yymmdd(what);
		}
    return;
  }
  // set0 分钟++并写入rtc
  if(ev == EV_KEY_SET_PRESS) {
    inc_and_write(what);
    return;
  } 
  
  // set1 分钟持续++
  if(ev == EV_KEY_SET_LPRESS) {
    if((lpress_start % LPRESS_INC_DELAY) == 0) {
      inc_only(what);
    }
    lpress_start++;
    if(lpress_start == LPRESS_INC_OVERFLOW) lpress_start = 0;
    return;
  }

  // set抬起停止++并写入rtc
  if(ev == EV_KEY_SET_UP) {
    write_only(what);
    lpress_start = 0;
    return;
  }
  
  // 每250ms读一下rtc，更新数据
  if(ev == EV_250MS) {
		if(lpress_lock_year_hour 
			|| lpress_lock_month_min
		  || lpress_lock_day_sec) {
			return;
		}
		if(what == IS_HOUR 
			|| what == IS_MIN 
			|| what == IS_SEC) {
			update_hhmmss();
		} else if(what == IS_YEAR 
			|| what == IS_MON 
			|| what == IS_DAY) {
			update_yymmdd();
		}
    return;
  }
}

void sm_clock_mod_time_submod0(unsigned char from, unsigned char to, enum task_events ev)
{
  CDBG("sm_clock_mod_time_submod0 %bd %bd %bd\n", from, to, ev);
  
	if(get_sm_ss_state(from) == SM_CLOCK_MODIFY_TIME_INIT 
		&& ev == EV_KEY_MOD_UP) {
			ev = EV_KEY_MOD_PRESS;
	}
	
	sm_clock_mod_time(IS_HOUR, ev);
}

void sm_clock_mod_time_submod1(unsigned char from, unsigned char to, enum task_events ev)
{
  CDBG("sm_clock_mod_time_submod1 %bd %bd %bd\n", from, to, ev);
	
	sm_clock_mod_time(IS_MIN, ev);
}

void sm_clock_mod_time_submod2(unsigned char from, unsigned char to, enum task_events ev)
{
  CDBG("sm_clock_mod_time_submod2 %bd %bd %bd\n", from, to, ev);
  
	sm_clock_mod_time(IS_SEC, ev);
}

void sm_clock_mod_time_submod3(unsigned char from, unsigned char to, enum task_events ev)
{
  CDBG("sm_clock_mod_time_submod3 %bd %bd %bd\n", from, to, ev);
  
	sm_clock_mod_time(IS_YEAR, ev);
}

void sm_clock_mod_time_submod4(unsigned char from, unsigned char to, enum task_events ev)
{
  CDBG("sm_clock_mod_time_submod4 %bd %bd %bd\n", from, to, ev);

	sm_clock_mod_time(IS_MON, ev);
}

void sm_clock_mod_time_submod5(unsigned char from, unsigned char to, enum task_events ev)
{
  CDBG("sm_clock_mod_time_submod5 %bd %bd %bd\n", from, to, ev);

	sm_clock_mod_time(IS_DAY, ev);
}
