#include "sm_clock_alarm.h"
#include "debug.h"
#include "alarm.h"
#include "led.h"
#include "beeper.h"
#include "power.h"
#include "clock.h"
#include "cext.h"
#include "mod_common.h"
#include "timer.h"

static void display_alarm(unsigned char what)
{
  unsigned char hour, min;
  bit is12;
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
        hour = alarm0_get_hour();
        min  = alarm0_get_min();
        is12 = alarm0_get_hour_12();
      } else {
        hour = clock_get_hour();
        min  = clock_get_min();
        is12 = clock_get_hour_12();
      }
      led_set_dp(1);
      led_set_dp(2);
      if(is12 && hour > 12) {
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
  }
}

void sm_clock_alarm(unsigned char from, unsigned char to, enum task_events ev)
{
  CDBG("sm_clock_alarm %bd %bd %bd\n", from, to, ev);
  
  // 闹钟0到时间了
  if(get_sm_ss_state(to) == SM_CLOCK_ALARM_HIT_ALARM0 && ev == EV_ALARM0) {
    clock_display(0);
    display_alarm(IS_ALARM0);
    beeper_play_music();
    return;
  }
  
  // 闹钟1到时间了
  if(get_sm_ss_state(to) == SM_CLOCK_ALARM_HIT_ALARM1 && ev == EV_ALARM1) {
    clock_display(0);
    display_alarm(IS_ALARM1);
    beeper_beep_beep_always();
    return;
  }
}