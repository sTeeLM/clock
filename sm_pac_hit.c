#include "sm_pac_hit.h"
#include "debug.h"
#include "alarm.h"
#include "led.h"
#include "beeper.h"
#include "power.h"
#include "clock.h"
#include "cext.h"
#include "mod_common.h"
#include "timer.h"

static void display_pac(unsigned char what)
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
    case IS_COUNTER:
        led_set_code(5, 'C');
        led_set_code(4, 'O');
        led_set_code(3, 'U');
        led_set_code(2, 'N');
        led_set_code(1, '0');
        led_set_code(0, '0');
      break; 
  }
}

void sm_pac_hit(unsigned char from, unsigned char to, enum task_events ev)
{
  CDBG("sm_pac_hit %bd %bd %bd\n", from, to, ev);
  
  // 到时间切换到时间显示模式
  if(get_sm_ss_state(to) == SM_PAC_HIT_ALARM0 && ev == EV_1S) {
    set_task(EV_KEY_SET_PRESS);
  }
  
  // 到时间切换到时间显示模式
  if(get_sm_ss_state(to) == SM_PAC_HIT_ALARM1 && ev == EV_1S) {
    set_task(EV_KEY_SET_PRESS);
  }
  
  // 到时间切换到时间显示模式
  if(get_sm_ss_state(to) == SM_PAC_HIT_COUNTER && ev == EV_1S) {
    set_task(EV_KEY_SET_PRESS);
  }
  
  // 闹钟0到时间了
  if(get_sm_ss_state(to) == SM_PAC_HIT_ALARM0 && ev == EV_ALARM0) {
    display_pac(IS_ALARM0);
    beeper_play_music();
    return;
  }
  
  // 闹钟1到时间了
  if(get_sm_ss_state(to) == SM_PAC_HIT_ALARM1 && ev == EV_ALARM1) {
    display_pac(IS_ALARM1);
    beeper_beep_beep_always();
    return;
  }

  // 倒计时到了
  if(get_sm_ss_state(to) == SM_PAC_HIT_COUNTER && ev == EV_COUNTER) {
    timer_set_led_autorefresh(0, TIMER_DISP_MODE_HHMMSS);
    display_pac(IS_COUNTER);
    beeper_beep_beep_always();
    return;
  } 

  // 该进入节电模式了
  if(get_sm_ss_state(to) == SM_PAC_HIT_POWERSAVE && ev == EV_POWER_SAVE) {
    beeper_beep_beep_always();
    power_enter_powersave();
    power_leave_powersave();
    return;
  } 
  
}