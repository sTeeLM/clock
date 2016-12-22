#include "sm_pac_hit.h"
#include "debug.h"
#include "led.h"
#include "rtc.h"
#include "beeper.h"
#include "power.h"
#include "timer.h"
#include "cext.h"
#include "mod_common.h"

static void display_pac(unsigned char what)
{
  unsigned char hour, min;
  led_clear();
  switch(what) {
    case IS_ALARM0:
    case IS_ALARM1: 
      if(what == IS_ALARM0) {
        led_set_code(5, 'A');
        led_set_code(4, 'L');
      } else {
        led_set_code(5, 'B');
        led_set_code(4, 'S');
      }
      if(what == IS_ALARM0) {
        rtc_read_data(RTC_TYPE_ALARM0);
        hour = rtc_alarm_get_hour();
        min  = rtc_alarm_get_min();
      } else { // 闹钟1是整点报时，会被自动设置，读取ALARM1不准确
        rtc_read_data(RTC_TYPE_TIME);
        hour = rtc_time_get_hour();
        min  = rtc_time_get_min();
      }
      led_set_dp(1);
      led_set_dp(2);
      if(rtc_alarm_get_hour_12() && hour > 12) {
        led_set_dp(3);
        hour -= 12;
      } else {
        led_clr_dp(3);
      }
      if((hour / 10) != 0) {
        led_set_code(3, (hour / 10) + 0x30);
      } else {
        led_set_code(3, LED_CODE_BLACK);
      }
      led_set_code(2, (hour % 10) + 0x30);
      led_set_code(1, (min / 10)+ 0x30);
      led_set_code(0, (min % 10) + 0x30); 
      break;
    case IS_COUNTER:
        led_set_dp(1);
        led_set_dp(2);
        led_set_code(5, 'C');
        led_set_code(4, 'U');
        led_set_code(3, '0');
        led_set_code(2, '0');
        led_set_code(3, '0');
        led_set_code(2, '0');
      break; 
  }
}

static void reset_switch(void)
{
  last_display_s = counter_1s;
}

static bit test_switch(unsigned char what)
{
  switch(what) {
    case IS_ALARM0:  what = 30; break;
    case IS_ALARM1:  what = 5; break; 
    case IS_COUNTER: what = 5; break; 
  }
  
  if(time_diff(counter_1s, last_display_s) >= what) {
    CDBG("test_autoswitch time out!\n");
    set_task(EV_KEY_SET_PRESS);
    return 1;
  }
  return 0;
}

static void enter_powersave(void)
{
  
}

static void leave_powersave(void)
{
  
}

void sm_pac_hit(unsigned char from, unsigned char to, enum task_events ev)
{
  CDBG("sm_pac_hit %bd %bd %bd\n", from, to, ev);
  
  // 到时间切换到时间显示模式
  if(get_sm_ss_state(to) == SM_PAC_HIT_ALARM0 && ev == EV_1S) {
    if(test_switch(IS_ALARM0)) {
      beeper_stop_music();
    }
  }
  
  // 到时间切换到时间显示模式
  if(get_sm_ss_state(to) == SM_PAC_HIT_ALARM1 && ev == EV_1S) {
    test_switch(IS_ALARM1);
  }
  
  // 到时间切换到时间显示模式
  if(get_sm_ss_state(to) == SM_PAC_HIT_COUNTER && ev == EV_1S) {
    test_switch(IS_COUNTER);
  }
  
  // 闹钟0到时间了
  if(get_sm_ss_state(to) == SM_PAC_HIT_ALARM0 && ev == EV_ALARM0) {
    display_pac(IS_ALARM0);
    reset_switch();
    beeper_play_music();
    return;
  }
  
  // 闹钟1到时间了
  if(get_sm_ss_state(to) == SM_PAC_HIT_ALARM1 && ev == EV_ALARM1) {
    display_pac(IS_ALARM1);
    reset_switch();
    beeper_beep();
    return;
  }

  // 倒计时到了
  if(get_sm_ss_state(to) == SM_PAC_HIT_COUNTER && ev == EV_COUNTER) {
    display_pac(IS_COUNTER);
    reset_switch();
    beeper_beep();
    return;
  } 

  // 该进入节电模式了
  if(get_sm_ss_state(to) == SM_PAC_HIT_POWERSAVE && ev == EV_POWER_SAVE) {
    beeper_beep();
    power_enter_powersave();
    power_leave_powersave();
    return;
  } 
  
}