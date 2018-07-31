#include "sm_clock_mod_time.h"
#include "led.h"
#include "rtc.h"
#include "clock.h"
#include "debug.h"
#include "cext.h"

#include "mod_common.h"

#ifdef __CLOCK_DEBUG__
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
#endif

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


static void enter_clock(unsigned char what) // blink hour:0, min:1, sec:2
{
  clock_display(1);
  led_clr_all_blink();
  switch(what) {
    case IS_HOUR:
    case IS_YEAR:
      led_set_blink(5);
      led_set_blink(4);    
      break;
    case IS_MIN:
    case IS_MON:
      led_set_blink(3);
      led_set_blink(2);    
      break;      
    case IS_SEC:
    case IS_DAY:
      led_set_blink(1);
      led_set_blink(0);    
      break;      
  }
  if(what == IS_HOUR || what == IS_MIN || what == IS_SEC)
    clock_switch_display_mode(CLOCK_DISPLAY_MODE_HHMMSS);
  else
    clock_switch_display_mode(CLOCK_DISPLAY_MODE_YYMMDD);
}

void sm_clock_mod_time_init(unsigned char from, unsigned char to, enum task_events ev)
{
#ifdef __CLOCK__DEBUG__
  CDBG(("sm_clock_mod_time_init %bu %bu %bu\n", from, to, ev));
#else
  UNUSED_PARAM(from);
  UNUSED_PARAM(to);
  UNUSED_PARAM(ev);
#endif
  clock_display(0);
  display_logo(DISPLAY_LOGO_TYPE_CLOCK, 1);
}

static void sm_clock_mod_time(unsigned char what, enum task_events ev)
{
  // 按mod0进入新模式
  if(ev == EV_KEY_MOD_PRESS) {
    enter_clock(what);
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
}

void sm_clock_mod_time_submod0(unsigned char from, unsigned char to, enum task_events ev)
{
  UNUSED_PARAM(to);
  
  if(get_sm_ss_state(from) == SM_CLOCK_MODIFY_TIME_INIT 
    && ev == EV_KEY_MOD_UP) {
      ev = EV_KEY_MOD_PRESS;
  }
  
  sm_clock_mod_time(IS_HOUR, ev);
}

void sm_clock_mod_time_submod1(unsigned char from, unsigned char to, enum task_events ev)
{
  UNUSED_PARAM(from);
  UNUSED_PARAM(to);

  sm_clock_mod_time(IS_MIN, ev);
}

void sm_clock_mod_time_submod2(unsigned char from, unsigned char to, enum task_events ev)
{
  UNUSED_PARAM(from);
  UNUSED_PARAM(to);
  
  sm_clock_mod_time(IS_SEC, ev);
}

void sm_clock_mod_time_submod3(unsigned char from, unsigned char to, enum task_events ev)
{
  UNUSED_PARAM(from);
  UNUSED_PARAM(to);
  
  sm_clock_mod_time(IS_YEAR, ev);
}

void sm_clock_mod_time_submod4(unsigned char from, unsigned char to, enum task_events ev)
{
  UNUSED_PARAM(from);
  UNUSED_PARAM(to);
  
  sm_clock_mod_time(IS_MON, ev);
}

void sm_clock_mod_time_submod5(unsigned char from, unsigned char to, enum task_events ev)
{
  UNUSED_PARAM(from);
  UNUSED_PARAM(to);
  
  sm_clock_mod_time(IS_DAY, ev);
}
