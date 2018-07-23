#include "sm_fuse_param.h"
#include "debug.h"
#include "mod_common.h"
#include "rom.h"
#include "led.h"
#include "lt_timer.h"
#include "clock.h"
#include "thermo.h"
#include "remote.h"
#include "indicator.h"
#include "cext.h"

const char * code sm_fuse_param_ss_name[] = 
{
  "SM_FUSE_PARAM_INIT",
  "SM_FUSE_PARAM_YY",
  "SM_FUSE_PARAM_MO",
  "SM_FUSE_PARAM_DD",
  "SM_FUSE_PARAM_HH",
  "SM_FUSE_PARAM_MM",
  "SM_FUSE_PARAM_SS",
  "SM_FUSE_PARAM_PASSWORD",
  NULL
};

#define password_index common_state
#define last_index     last_display_s

static void inc_only(unsigned char what)
{
  
  if(what >= IS_PASSWORD && what <= IS_PASSWORD5) {
    last_index = ++ last_index % 10;
    return;
  }
  
  switch (what) {
    case IS_HOUR:
    case IS_YEAR:
      if(!lpress_lock_year_hour) {
        lpress_lock_year_hour = 1;
        led_clr_blink(5);
        led_clr_blink(4); 
      }
      if(what == IS_HOUR) {
        lt_timer_inc_hour();
      } else {
        lt_timer_inc_year();
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
        lt_timer_inc_min();
      } else {
        lt_timer_inc_month();
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
        lt_timer_inc_sec();
      } else {
        lt_timer_inc_date();
      }
      break;
  }
}

static void write_only(unsigned char what)
{
  unsigned char i;
  
  if(what >= IS_PASSWORD && what <= IS_PASSWORD5) {
    i = what - IS_PASSWORD;
    rom_write(ROM_FUSE_PASSWORD + i, last_index);
    return;
  }
  
  switch(what) {
    case IS_HOUR:
      if(lpress_lock_year_hour == 1) {
        lt_timer_save_to_rom(LT_TIMER_SYNC_HOUR);
        lpress_lock_year_hour = 0;
        led_set_blink(5);
        led_set_blink(4);
      }
      break;
    case IS_MIN:
      if(lpress_lock_month_min == 1) {
        lt_timer_save_to_rom(LT_TIMER_SYNC_MIN);
        lpress_lock_month_min = 0;
        led_set_blink(3);
        led_set_blink(2); 
      }
      break; 
    case IS_SEC:
      if(lpress_lock_day_sec == 1) {
        lt_timer_save_to_rom(LT_TIMER_SYNC_SEC);
        lpress_lock_day_sec = 0;
        led_set_blink(1);
        led_set_blink(0); 
      }
      break;
    case IS_DAY:
      if(lpress_lock_day_sec == 1) {
        lt_timer_save_to_rom(LT_TIMER_SYNC_DATE);
        lpress_lock_day_sec = 0;
        led_set_blink(1);
        led_set_blink(0); 
      }
      break;
    case IS_MON:
      if(lpress_lock_month_min == 1) {
        lt_timer_save_to_rom(LT_TIMER_SYNC_MONTH);
        lpress_lock_month_min = 0;
        led_set_blink(3);
        led_set_blink(2);      
      }
      break;
    case IS_YEAR:
      if(lpress_lock_year_hour == 1) {
        lt_timer_save_to_rom(LT_TIMER_SYNC_YEAR);
        lpress_lock_year_hour = 0;
        led_set_blink(5);
        led_set_blink(4);
      }
      break;   
  }
  
}

static void inc_and_write(unsigned char what)
{
  inc_only(what);
  write_only(what);
}

static void update_time(unsigned char what)
{
  unsigned char year, mon, date,hour, min, sec;
  
  if(what == IS_YEAR || what == IS_MON || what == IS_DAY) {
  
    year = lt_timer_get_year();
    mon  = lt_timer_get_month();
    date = lt_timer_get_date();
    
    led_set_dp(2);
    led_set_dp(4);

    CDBG("update_yymmdd %bu-%bu-%bu\n", year, mon, date);
    
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
  } else {
    hour = lt_timer_get_hour();
    min  = lt_timer_get_min();
    sec  = lt_timer_get_sec();
    
    // 两个“:”号
    led_set_dp(1);
    led_set_dp(2);
    led_set_dp(3);
    led_set_dp(4); 
    
    // 如果是12小时显示，以第一位数字的点表示“PM”
    if(lt_timer_get_hour_12() && hour > 12) {
      led_set_dp(5);
      hour -= 12;
    } else if(lt_timer_get_hour_12() && hour == 12){
      led_set_dp(5);
    } else {
      led_clr_dp(5);
    }
    
    
    CDBG("update_hhmmss %bu:%bu:%bu\n", hour, min, sec);  
    
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
}

static void enter_time(unsigned char what) // blink year:0, month:1, day:2
{
  led_clear();
  switch(what) {
    case IS_YEAR:
    case IS_HOUR:
      led_set_blink(5);
      led_set_blink(4);    
      break;
    case IS_MON:
    case IS_MIN:
      led_set_blink(3);
      led_set_blink(2);    
      break;      
    case IS_DAY:
    case IS_SEC:
      led_set_blink(1);
      led_set_blink(0);    
      break;      
  }
  update_time(what); 
}

static void update_password(unsigned char index)
{
  unsigned char i;
  i = index - IS_PASSWORD;
  led_clear();
  led_set_code(5, '-');
  led_set_code(4, '-');
  led_set_code(3, '-');
  led_set_code(2, '-');
  led_set_code(1, '-');
  led_set_code(0, '-');
  led_set_code(i, last_index + 0x30);
}

static void enter_password(unsigned char index)
{
  unsigned char i;
  i = index - IS_PASSWORD;
  last_index = rom_read(ROM_FUSE_PASSWORD + i);
  update_password(index);
}

void sm_fuse_param_init(unsigned char from, unsigned char to, enum task_events ev) 
{
  UNUSED_PARAM(from);
  UNUSED_PARAM(to);
  
  display_logo(DISPLAY_LOGO_TYPE_FUSE, 1);

  indicator_clr();
  indicator_set(INDICATOR_COLOR_RED, INDICATOR_MODE_BLINK);
  indicator_set(INDICATOR_COLOR_GREEN, INDICATOR_MODE_BLINK);
  
  if(ev == EV_KEY_V0) {
    set_task(EV_KEY_SET_UP);
  } 
}

static void sm_fuse_param_set_time(unsigned char what, unsigned char ev)
{
  if(ev == EV_KEY_MOD_PRESS) {
    enter_time(what);
    return;
  }
  
  if(ev == EV_KEY_SET_PRESS) {
    inc_and_write(what);
    update_time(what);
    return;
  }
  
  if(ev == EV_KEY_SET_LPRESS) {
    if((lpress_start % LPRESS_INC_DELAY) == 0) {
      inc_only(what);
      update_time(what);
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


void sm_fuse_param_submod0(unsigned char from, unsigned char to, enum task_events ev) 
{ 
  UNUSED_PARAM(to);
  
  if((ev == EV_KEY_SET_UP && get_sm_ss_state(from) == SM_FUSE_PARAM_INIT)
    || (ev == EV_KEY_V0 && get_sm_ss_state(from) == SM_FUSE_PARAM_PASSWORD)) {
    ev = EV_KEY_MOD_PRESS;
  }
  password_index = 5;
  lt_timer_load_from_rom();
  sm_fuse_param_set_time(IS_YEAR, ev);
}

void sm_fuse_param_submod1(unsigned char from, unsigned char to, enum task_events ev) 
{
  UNUSED_PARAM(from);
  UNUSED_PARAM(to);
  
  sm_fuse_param_set_time(IS_MON, ev);
}

void sm_fuse_param_submod2(unsigned char from, unsigned char to, enum task_events ev) 
{
  UNUSED_PARAM(from);
  UNUSED_PARAM(to);
  
  sm_fuse_param_set_time(IS_DAY, ev);
}

void sm_fuse_param_submod3(unsigned char from, unsigned char to, enum task_events ev) 
{ 
  UNUSED_PARAM(from);
  UNUSED_PARAM(to);
  
  sm_fuse_param_set_time(IS_HOUR, ev);
}

void sm_fuse_param_submod4(unsigned char from, unsigned char to, enum task_events ev) 
{
  UNUSED_PARAM(from);
  UNUSED_PARAM(to);
  
  sm_fuse_param_set_time(IS_MIN, ev);
}

void sm_fuse_param_submod5(unsigned char from, unsigned char to, enum task_events ev) 
{
  UNUSED_PARAM(from);
  UNUSED_PARAM(to);
  
  sm_fuse_param_set_time(IS_SEC, ev);
}

void sm_fuse_param_submod6(unsigned char from, unsigned char to, enum task_events ev) 
{ 
  UNUSED_PARAM(from);
  UNUSED_PARAM(to);
  
  // 设置password
  if(ev == EV_KEY_MOD_PRESS) {
    if(get_sm_ss_state(from) == SM_FUSE_PARAM_PASSWORD) {
      if(password_index > 0) {
        password_index --;
      } else {
        set_task(EV_KEY_V0);
        return;
      }
    }
    enter_password(IS_PASSWORD + password_index); 
    return;
  }
  
  if(ev == EV_KEY_SET_PRESS) {
    inc_and_write(IS_PASSWORD + password_index);
    update_password(IS_PASSWORD + password_index);
    return;
  }
  
  if(ev == EV_KEY_SET_LPRESS) {
    if((lpress_start % LPRESS_INC_DELAY) == 0) {
      inc_only(IS_PASSWORD + password_index);
      update_password(IS_PASSWORD + password_index);
    }
    lpress_start++;
    if(lpress_start == LPRESS_INC_OVERFLOW) lpress_start = 0;
    return;
  } 

  if(ev == EV_KEY_SET_UP) {
    write_only(IS_PASSWORD + password_index);
    lpress_start = 0;
    return;
  }
}
