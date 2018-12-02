#include "sm_clock_mod_alarm.h"
#include "mod_common.h"
#include "clock.h"
#include "alarm.h"
#include "led.h"
#include "debug.h"
#include "beeper.h"
#include "cext.h"

#ifdef __CLOCK_DEBUG__
const char * code sm_clock_mod_alarm_ss_name[] = 
{
  "SM_CLOCK_MODIFY_ALARM_INIT",
  "SM_CLOCK_MODIFY_ALARM_HH",
  "SM_CLOCK_MODIFY_ALARM_MM",
  "SM_CLOCK_MODIFY_ALARM_DAY",
  "SM_CLOCK_MODIFY_ALARM_BS",
  "SM_CLOCK_MODIFY_ALARM_DUR",
  NULL
};
#endif

#define alarm_index common_state

#define val last_display_s

static void update_alarm(unsigned char what, unsigned char day)
{
  switch (what) {
    case IS_HOUR:
      if(alarm0_get_hour_12() && val > 12) {
        led_set_dp(3);
        val -= 12;
      } else if(alarm0_get_hour_12() && val == 12){
        led_set_dp(3);
      } else {
        led_clr_dp(3);
      }
      if((val / 10) != 0) {
        led_set_code(3, (val / 10) + 0x30);
      } else {
        led_set_code(3, LED_CODE_BLACK);
      }
      led_set_code(2, (val % 10) + 0x30);
    break;
    case IS_MIN:
      led_set_code(1, (val / 10)+ 0x30);
      led_set_code(0, (val % 10) + 0x30); 
    break;
    case IS_ONOFF:
      led_set_code(3, day + 0x30);
      if(val) {
        led_set_code(2, LED_CODE_BLACK);
        led_set_code(1, 'O');
        led_set_code(0, 'N');
      } else {
        led_set_code(2, 'O');
        led_set_code(1, 'F');
        led_set_code(0, 'F');
      }
    break;
    case IS_BS:
      if(val) {
        led_set_code(2, LED_CODE_BLACK);
        led_set_code(1, 'O');
        led_set_code(0, 'N');
      } else {
        led_set_code(2, 'O');
        led_set_code(1, 'F');
        led_set_code(0, 'F');
      }
    break;
    case IS_ALARM_DUR:
      led_set_code(2, LED_CODE_BLACK);
      led_set_code(1, (val / 10) + 0x30);
      led_set_code(0, (val % 10) + 0x30);
    break;
  }
}



static void enter_alarm(unsigned char what, unsigned char day)
{
  led_clear();

  if(what == IS_HOUR || what == IS_MIN || what == IS_ONOFF) {
    led_set_code(5, 'A');
    led_set_code(4, 'L'); 
    if(what != IS_ONOFF) {
      led_set_dp(1);
      led_set_dp(2);
    }
  } else if(what == IS_BS) {
    led_set_code(5, 'B');
    led_set_code(4, 'S');
  } else if(what == IS_ALARM_DUR) {
    led_set_code(5, 'B');
    led_set_code(4, 'T');
    led_set_code(3, 'O');
  }

  switch(what) {
    case IS_HOUR:
      led_set_blink(3);
      led_set_blink(2);
      val = alarm0_get_min();
      update_alarm(IS_MIN, 0);
      val = alarm0_get_hour();
      break;
    case IS_MIN:
      led_set_blink(1);
      led_set_blink(0);
      val = alarm0_get_hour();
      update_alarm(IS_HOUR, 0);
      val = alarm0_get_min();
      break;      
    case IS_ONOFF:
      led_set_blink(2);
      led_set_blink(1);
      led_set_blink(0);
      val = alarm0_test_enable(day);
      break; 
    case IS_ALARM_DUR:
      led_set_blink(1);
      led_set_blink(0);
      val = alarm0_get_dur();
      break;
    case IS_BS:
      led_set_blink(2);
      led_set_blink(1);
      led_set_blink(0);
      val = alarm1_test_enable();
      break;
  }
  
  update_alarm(what, day);
}


static void write_only(unsigned char what, unsigned char day)
{
  UNUSED_PARAM(day);

  switch(what) {
    case IS_HOUR:
      if(lpress_lock_year_hour == 1) {
        lpress_lock_year_hour = 0;
        led_set_blink(3);
        led_set_blink(2); 
        alarm0_sync_to_rtc();
        alarm_save_rom(ALARM_SYNC_ALARM0_HOUR);
      }
      break;
    case IS_MIN:
      if(lpress_lock_month_min == 1) {
        lpress_lock_month_min = 0;
        led_set_blink(1);
        led_set_blink(0);
        alarm0_sync_to_rtc();
        alarm_save_rom(ALARM_SYNC_ALARM0_MIN);
      }
      break;
    case IS_ONOFF:
      alarm0_sync_to_rtc();
      alarm_save_rom(ALARM_SYNC_ALARM0_DAY_MASK);
      break;
    case IS_BS:
      alarm1_sync_to_rtc();
      alarm_save_rom(ALARM_SYNC_ALARM1_ENABLE);
    break;
    case IS_ALARM_DUR:
      alarm_save_rom(ALARM_SYNC_ALARM0_DUR);
    break;
  } 
}

static void inc_only(unsigned char what, unsigned char day)
{
  switch (what) {
    case IS_HOUR:
      if(!lpress_lock_year_hour) {
        lpress_lock_year_hour = 1;
        led_clr_blink(3);
        led_clr_blink(2);        
      }
      alarm0_inc_hour();
      val = alarm0_get_hour();
      break;
    case IS_MIN:
      if(!lpress_lock_month_min) {
        lpress_lock_month_min = 1;
        led_clr_blink(1);
        led_clr_blink(0); 
      }
      alarm0_inc_min();
      val = alarm0_get_min();
      break;
    case IS_ONOFF:
      alarm0_set_enable(day, !alarm0_test_enable(day));
      val = alarm0_test_enable(day);
    break;
    case IS_BS:
      alarm1_set_enable(!alarm1_test_enable());
      val = alarm1_test_enable();
    break;
    case IS_ALARM_DUR:
      alarm0_inc_dur();
      val = alarm0_get_dur();
      break;
  }
  update_alarm(what, day);
}

static void inc_write(unsigned char what, day)
{
  inc_only(what, day);
  write_only(what, day);
}

void sm_clock_mod_alarm_init(unsigned char from, unsigned char to, enum task_events ev)
{
  UNUSED_PARAM(from);
  UNUSED_PARAM(to);
  UNUSED_PARAM(ev);
  
  clock_display(0);
  display_logo(DISPLAY_LOGO_TYPE_CLOCK, 2);
}

void sm_clock_mod_alarm_submod0(unsigned char from, unsigned char to, enum task_events ev)
{
  UNUSED_PARAM(from);
  UNUSED_PARAM(to);
  
 // 切换到修改闹钟
  if((get_sm_ss_state(from) == SM_CLOCK_MODIFY_ALARM_INIT 
    && ev == EV_KEY_MOD_UP) || (get_sm_ss_state(from) == SM_CLOCK_MODIFY_ALARM_DUR && ev == EV_KEY_MOD_PRESS)) {
    enter_alarm(IS_HOUR, 0);
    return;
  }
  
  // set0小时++ 
  if(ev == EV_KEY_SET_PRESS) {
    inc_write(IS_HOUR, 0);
    return;
  }
  
  // set1小时持续++
  if(ev == EV_KEY_SET_LPRESS) {
    if((lpress_start % LPRESS_INC_DELAY) == 0) {
      inc_only(IS_HOUR, 0);
    }
    lpress_start++;
    if(lpress_start == LPRESS_INC_OVERFLOW) lpress_start = 0;
    return;
  }
  
  // set抬起停止++，写入rtc
  if(ev == EV_KEY_SET_UP) {
    write_only(IS_HOUR, 0);
    lpress_start = 0;
    return;
  }
}

void sm_clock_mod_alarm_submod1(unsigned char from, unsigned char to, enum task_events ev)
{
  UNUSED_PARAM(from);
  UNUSED_PARAM(to);  
  
  // mod0进入修改分钟模式
  if(ev == EV_KEY_MOD_PRESS) {
    enter_alarm(IS_MIN, 0);
    return;
  } 
  
  // set0分钟++
  if(ev == EV_KEY_SET_PRESS) {
    inc_write(IS_MIN, 0);
    return;
  }
  
  // set1分钟持续++
  if(ev == EV_KEY_SET_LPRESS) {
    if((lpress_start % LPRESS_INC_DELAY) == 0) {
      inc_only(IS_MIN, 0);
    }
    lpress_start++;
    if(lpress_start == LPRESS_INC_OVERFLOW) lpress_start = 0;
    return;
  }
  
  // set抬起停止++，写入rtc 
  if(ev == EV_KEY_SET_UP) {
    write_only(IS_MIN, 0);
    lpress_start = 0;
    return;
  }
}

void sm_clock_mod_alarm_submod2(unsigned char from, unsigned char to, enum task_events ev)
{
  UNUSED_PARAM(from);
  UNUSED_PARAM(to);
  
  if(get_sm_ss_state(from) == SM_CLOCK_MODIFY_ALARM_MM) {
    alarm_index = 1;
    enter_alarm(IS_ONOFF, alarm_index);
    return;
  }
  
  if(ev == EV_KEY_SET_PRESS) {
    inc_write(IS_ONOFF, alarm_index);
    return;
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
  UNUSED_PARAM(from);
  UNUSED_PARAM(to);
  
  if(ev == EV_KEY_V0) {
    enter_alarm(IS_BS, 0);
    return;
  }
  
  if(ev == EV_KEY_SET_PRESS) {
    inc_write(IS_BS, 0);
    return;
  }
}

void sm_clock_mod_alarm_submod4(unsigned char from, unsigned char to, enum task_events ev)
{
  UNUSED_PARAM(from);
  UNUSED_PARAM(to);
  
  if(ev == EV_KEY_MOD_PRESS) {
    enter_alarm(IS_ALARM_DUR, 0);
    return;
  } 
  
  if(ev == EV_KEY_SET_PRESS) {
    inc_write(IS_ALARM_DUR, 0);
    return;
  }
}
