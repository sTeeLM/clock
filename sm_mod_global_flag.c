#include "sm_mod_global_flag.h"
#include "mod_common.h"
#include "led.h"
#include "rtc.h"
#include "debug.h"

static void display_global_flag(unsigned char what)
{
  led_clear();
  switch(what) {
    case IS_PS:
      led_set_code(5, 'P');
      led_set_code(4, 'S');
      if(powersave_timeout == 0) {
        led_set_code(2, 'O');
        led_set_code(1, 'F');
        led_set_code(0, 'F');
      } else {
        led_set_code(1, (powersave_timeout / 10) + 0x30);
        led_set_code(0, (powersave_timeout % 10) + 0x30);
      }
      break;
    case IS_BS:
      led_set_code(5, 'B');
      led_set_code(4, 'S');
      if(baoshi) {
        led_set_code(2, LED_CODE_BLACK);
        led_set_code(1, 'O');
        led_set_code(0, 'N');
      } else {
        led_set_code(2, 'O');
        led_set_code(1, 'F');
        led_set_code(0, 'F');
      }
      break;
    case IS_MUSIC:
      led_set_code(5, 'A');
      led_set_code(4, 'L');
      led_set_code(3, 'B');
      led_set_code(2, 'E');
      led_set_code(1, 'P');    
      led_set_code(0, alarm_music_index + 1 + 0x30);
      break;
    case IS_1224:
      led_set_code(5, 'D');
      led_set_code(4, 'S');
      led_set_code(3, 'P');    
      if(is_24) {
        led_set_code(1, '2');
        led_set_code(0, '4');
      } else {
        led_set_code(1, '1');
        led_set_code(0, '2');
      }
      break;
  }
}

static void inc_only(unsigned char what)
{
  switch(what) {
    case IS_PS:
      powersave_timeout += POWERSAVE_TIMEOUT_INTERVAL;
      if(powersave_timeout > MAX_POWERSAVE_TIMEOUT) {
        powersave_timeout = 0;
      }
      break;
    case IS_BS:
      baoshi = baoshi ? 0:1;
      break;
    case IS_MUSIC:
      alarm_music_index ++;
      if(alarm_music_index >= ALARM_MUSIC_CNT) {
        alarm_music_index = 0;
      }
      break;
    case IS_1224:
      is_24 = is_24 ? 0:1;
      rtc_read_data(RTC_TYPE_TIME);
      rtc_time_set_hour_12(is_24 ? 0 : 1);
      rtc_write_data(RTC_TYPE_TIME);
      rtc_read_data(RTC_TYPE_ALARM0);
      rtc_alarm_set_hour_12(is_24 ? 0 : 1);
      rtc_write_data(RTC_TYPE_ALARM0);
      break;
  }
}


void sm_mod_global_flag(unsigned char from, unsigned char to, enum task_events ev)
{
  CDBG("sm_mod_global_flag %bd %bd %bd\n", from, to, ev);
  
  // 按mod1进入修改全局标志位模式
  if(get_sm_ss_state(to) == SM_MODIFY_GLOBAL_FLAG_INIT && ev == EV_KEY_MOD_LPRESS) {
    display_logo(3);
    return;
  }

  // 切换到修改全局标志位模式
  if(get_sm_ss_state(from) == SM_MODIFY_GLOBAL_FLAG_INIT 
    && get_sm_ss_state(to) == SM_MODIFY_GLOBAL_FLAG_PS
    && ev == EV_KEY_MOD_UP) {
    display_global_flag(IS_PS);
    return;
  }

  // set0 省电模式超时时间设置
  if(get_sm_ss_state(to) == SM_MODIFY_GLOBAL_FLAG_PS && ev == EV_KEY_SET_PRESS) {
    inc_only(IS_PS);
    display_global_flag(IS_PS);
    return;
  }
  
  // mod0 进入设置整点报时on/off状态
  if(get_sm_ss_state(to) == SM_MODIFY_GLOBAL_FLAG_BS && ev == EV_KEY_MOD_PRESS) {
    display_global_flag(IS_BS);
    return;
  }
  
  // set0 整点报时on/off
  if(get_sm_ss_state(to) == SM_MODIFY_GLOBAL_FLAG_BS && ev == EV_KEY_SET_PRESS) {
    inc_only(IS_BS);
    display_global_flag(IS_BS);
    return;
  }  
  
  // mod0 进入闹铃音乐选择
  if(get_sm_ss_state(to) == SM_MODIFY_GLOBAL_FLAG_ALARM_BEEP && ev == EV_KEY_MOD_PRESS) {
    display_global_flag(IS_MUSIC);
    return;
  }

  // set0 闹铃音乐设置
  if(get_sm_ss_state(to) == SM_MODIFY_GLOBAL_FLAG_ALARM_BEEP && ev == EV_KEY_SET_PRESS) {
    inc_only(IS_MUSIC);
    display_global_flag(IS_MUSIC);
    return;
  }
  
  // mod0 进入1224小时设置状态
  if(get_sm_ss_state(to) == SM_MODIFY_GLOBAL_FLAG_1224 && ev == EV_KEY_MOD_PRESS) {
    display_global_flag(IS_1224);
    return;
  }
  
  // set0 1224模式切换
  if(get_sm_ss_state(to) == SM_MODIFY_GLOBAL_FLAG_1224 && ev == EV_KEY_SET_PRESS) {
    inc_only(IS_1224);
    display_global_flag(IS_1224);
    return;
  }
  
  // mod0 进入设置省电模式状态
  if(get_sm_ss_state(to) == SM_MODIFY_GLOBAL_FLAG_PS && ev == EV_KEY_MOD_PRESS) {
    display_global_flag(IS_PS);
    return;
  }
}