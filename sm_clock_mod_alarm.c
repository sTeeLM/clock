#include "sm_clock_mod_alarm.h"
#include "mod_common.h"
#include "clock.h"
#include "alarm.h"
#include "led.h"
#include "debug.h"

static void update_alarm(unsigned char what, unsigned char day)
{
  unsigned char hour, min;

  led_set_code(5, 'A');
  led_set_code(4, 'L');     

  if(what == IS_HOUR || what == IS_MIN) {
    
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
    
  } else {
     CDBG("update_alarm %s\n", alarm0_test_enable(day)? "ON":"OFF");
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
  }
}

static void toggle_alarm(unsigned char day)
{
  alarm0_set_enable(day, !alarm0_test_enable(day));
  update_alarm(IS_DAY_ONOFF, day);
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
    case IS_DAY_ONOFF:
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
      break;
    case IS_MIN:
      if(lpress_lock_month_min == 1) {
        lpress_lock_month_min = 0;
        led_set_blink(1);
        led_set_blink(0); 
      }
      break;
    case IS_DAY_ONOFF:
      break;
  } 
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
    if(lpress_start == 249) lpress_start = 0;
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
    if(lpress_start == 249) lpress_start = 0;
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
