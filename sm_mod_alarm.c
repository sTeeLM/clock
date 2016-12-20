#include "sm_mod_alarm.h"
#include "mod_common.h"
#include "rtc.h"
#include "led.h"
#include "debug.h"

static void update_alarm(unsigned char what)
{
  unsigned char hour, min;

  led_set_code(5, 'A');
  led_set_code(4, 'L');     

  if(what == IS_HOUR || what == IS_MIN) {
    
    rtc_read_data(RTC_TYPE_ALARM0);
    hour = rtc_alarm_get_hour();
    min  = rtc_alarm_get_min();
    led_set_dp(1);
    led_set_dp(2);
    
    if(lpress_lock_year_hour) {
      hour = year_hour;
    } else {
      year_hour = hour;
    }
    
    // 如果是12小时显示，以第三位数字的点表示“PM”
    if(rtc_alarm_get_hour_12() && hour > 12) {
      led_set_dp(3);
      hour -= 12;
    } else {
      led_clr_dp(3);
    }
    
    if(lpress_lock_month_min) {
      min = month_min;
    } else {
      month_min = min;
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
    rtc_read_data(RTC_TYPE_CTL);
     CDBG("update_alarm %s\n", rtc_test_alarm_int(0)? "ON":"OFF");
    if(rtc_test_alarm_int(0)) {
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

static void enter_alarm(unsigned char what)
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
      break;      
  }
  update_alarm(what);
}


static void write_only(unsigned char what)
{
  switch(what) {
    case IS_HOUR:
      if(lpress_lock_year_hour == 1) {
        rtc_alarm_set_hour(year_hour);
        rtc_write_data(RTC_TYPE_ALARM0);
        lpress_lock_year_hour = 0;
        led_set_blink(3);
        led_set_blink(2); 
      }
      break;
    case IS_MIN:
      if(lpress_lock_month_min == 1) {
        rtc_alarm_set_min(month_min);
        rtc_write_data(RTC_TYPE_ALARM0);
        lpress_lock_month_min = 0;
        led_set_blink(1);
        led_set_blink(0); 
      }
      break;
    case IS_ONOFF:
      rtc_write_data(RTC_TYPE_CTL);
      break;
  }  
}

static void inc_only(unsigned char what)
{
  bit enabled;
  switch (what) {
    case IS_HOUR:
      if(!lpress_lock_year_hour) {
        lpress_lock_year_hour = 1;
        led_clr_blink(3);
        led_clr_blink(2);        
        rtc_read_data(RTC_TYPE_ALARM0);
        year_hour = rtc_alarm_get_hour();
      }
      year_hour = (year_hour + 1)% 24;
      break;
    case IS_MIN:
      if(!lpress_lock_month_min) {
        lpress_lock_month_min = 1;
        led_clr_blink(1);
        led_clr_blink(0); 
        rtc_read_data(RTC_TYPE_ALARM0);
        month_min = rtc_alarm_get_min();
      }
      month_min = (month_min + 1) % 60;
      break;
    case IS_ONOFF:
      enabled = rtc_test_alarm_int(0);
      rtc_enable_alarm_int(enabled ? 0:1, 0);
      break;
  }
}

static void inc_write(unsigned char what)
{
  inc_only(what);
  write_only(what);
}


void sm_mod_alarm(unsigned char from, unsigned char to, enum task_events ev)
{
  CDBG("sm_mod_alarm %bd %bd %bd\n", from, to, ev);
  
  // 按mod1进入修改闹钟模式
  if(get_sm_ss_state(to) == SM_MODIFY_ALARM_INIT && ev == EV_KEY_MOD_LPRESS) {
    display_logo(2);
    return;
  }  
  
  // 切换到修改时间大模式
  if(get_sm_ss_state(from) == SM_MODIFY_ALARM_INIT 
    && get_sm_ss_state(to) == SM_MODIFY_ALARM_HH
    && ev == EV_KEY_MOD_UP) {
    enter_alarm(IS_HOUR);
    return;
  }
  
  // set0小时++ 
  if(get_sm_ss_state(to) == SM_MODIFY_ALARM_HH && ev == EV_KEY_SET_PRESS) {
    inc_write(IS_HOUR);
    return;
  }
  
  // set1小时持续++
  if(get_sm_ss_state(to) == SM_MODIFY_ALARM_HH && ev == EV_KEY_SET_LPRESS) {
    if((lpress_start % LPRESS_INC_DELAY) == 0) {
      inc_only(IS_HOUR);
    }
    lpress_start++;
    if(lpress_start == 249) lpress_start = 0;
    return;
  }
  
  // set抬起停止++，写入rtc
  if(get_sm_ss_state(to) == SM_MODIFY_ALARM_HH && ev == EV_KEY_SET_UP) {
    write_only(IS_HOUR);
    lpress_start = 0;
    return;
  }
  
  // mod0进入修改分钟模式
  if(get_sm_ss_state(to) == SM_MODIFY_ALARM_MM && ev == EV_KEY_MOD_PRESS) {
    enter_alarm(IS_MIN);
    return;
  } 
  
  // 每250ms读取一下rtc
  if(get_sm_ss_state(to) == SM_MODIFY_ALARM_HH && ev == EV_250MS) {
    update_alarm(IS_HOUR);
    return;
  }
  
  // 每250ms读取一下rtc
  if(get_sm_ss_state(to) == SM_MODIFY_ALARM_MM && ev == EV_250MS) {
    update_alarm(IS_MIN);
    return;
  }
  
  // set0分钟++
  if(get_sm_ss_state(to) == SM_MODIFY_ALARM_MM && ev == EV_KEY_SET_PRESS) {
    inc_write(IS_MIN);
    return;
  }
  
  // set1分钟持续++
  if(get_sm_ss_state(to) == SM_MODIFY_ALARM_MM && ev == EV_KEY_SET_LPRESS) {
    if((lpress_start % LPRESS_INC_DELAY) == 0) {
      inc_only(IS_MIN);
    }
    lpress_start++;
    if(lpress_start == 249) lpress_start = 0;
    return;
  }
  
  // set抬起停止++，写入rtc 
  if(get_sm_ss_state(to) == SM_MODIFY_ALARM_MM && ev == EV_KEY_SET_UP) {
    write_only(IS_MIN);
    lpress_start = 0;
    return;
  }
  
  // 每250ms读取一下rtc
  if(get_sm_ss_state(to) == SM_MODIFY_ALARM_ON && ev == EV_250MS) {
    update_alarm(IS_ONOFF);
    return;
  }
  
  // mod0进入打开关闭状态
  if(get_sm_ss_state(to) == SM_MODIFY_ALARM_ON && ev == EV_KEY_MOD_PRESS) {
    enter_alarm(IS_ONOFF);
    return;
  }
  
  // set0 调整打开关闭，并写入rtc
  if(get_sm_ss_state(to) == SM_MODIFY_ALARM_ON && ev == EV_KEY_SET_PRESS) {
    inc_write(IS_ONOFF);
    return;
  }
  
  // 每250ms读取一下rtc
  if(get_sm_ss_state(to) == SM_MODIFY_ALARM_ON && ev == EV_250MS) {
    update_alarm(IS_ONOFF);
    return;
  }
}
