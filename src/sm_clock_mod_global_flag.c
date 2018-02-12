#include "sm_clock_mod_global_flag.h"
#include "mod_common.h"
#include "led.h"
#include "beeper.h"
#include "power.h"
#include "clock.h"
#include "alarm.h"
#include "debug.h"
#include "rom.h"

const char * code sm_clock_mod_global_flag_name[] = 
{
  "SM_CLOCK_MODIFY_GLOBAL_FLAG_INIT",
  "SM_CLOCK_MODIFY_GLOBAL_FLAG_PS",
  "SM_CLOCK_MODIFY_GLOBAL_FLAG_BS",
  "SM_CLOCK_MODIFY_GLOBAL_FLAG_ALARM_MUSIC",
  "SM_CLOCK_MODIFY_GLOBAL_FLAG_BEEP",
  "SM_CLOCK_MODIFY_GLOBAL_FLAG_1224",
  NULL
};

static void display_global_flag(unsigned char what)
{
  bit baoshi,is_24;

  baoshi = alarm1_test_enable();

  is_24 = !clock_get_hour_12();
  
  led_clear();
  
  CDBG("display_global_flag %bd\n", what);
  
  switch(what) {
    case IS_PS:
      led_set_code(5, 'P');
      led_set_code(4, 'S');
      if(power_get_powersave_to_s() == 0) {
        led_set_code(2, 'O');
        led_set_code(1, 'F');
        led_set_code(0, 'F');
      } else {
        led_set_code(1, (power_get_powersave_to_s() / 10) + 0x30);
        led_set_code(0, (power_get_powersave_to_s() % 10) + 0x30);
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
      led_set_code(5, 'S');
      led_set_code(4, 'O');
      led_set_code(3, 'U');
      led_set_code(2, 'N');
      led_set_code(1, 'D');  
      CDBG("beeper_get_music_index return %bd\n", beeper_get_music_index());
      led_set_code(0, beeper_get_music_index() + 1 + 0x30);
      break;
    case IS_BEEP:
      led_set_code(5, 'B');
      led_set_code(4, 'E');
      led_set_code(3, 'P');
      if(beeper_get_beep_enable()) {
        led_set_code(2, LED_CODE_BLACK);
        led_set_code(1, 'O');
        led_set_code(0, 'N');        
      } else {
        led_set_code(2, 'O');
        led_set_code(1, 'F');
        led_set_code(0, 'F');
      }
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

static void inc_write(unsigned char what)
{
  switch(what) {
    case IS_PS:
      power_inc_powersave_to();
      rom_write(ROM_POWERSAVE_TO, power_get_powersave_to());
      break;
    case IS_BS:
      alarm1_set_enable(!alarm1_test_enable());
      alarm1_sync_to_rtc();
      rom_write(ROM_ALARM1_ENABLE, alarm1_test_enable() ? 1 : 0);
      break;
    case IS_MUSIC:
      beeper_inc_music_index();
      rom_write(ROM_BEEPER_MUSIC_INDEX, beeper_get_music_index());
      break;
    case IS_BEEP:
      beeper_set_beep_enable(!beeper_get_beep_enable());
    rom_write(ROM_BEEPER_ENABLE, beeper_get_beep_enable() ? 1 : 0);
      break;
    case IS_1224:
      clock_set_hour_12(!clock_get_hour_12());
      clock_sync_to_rtc(CLOCK_SYNC_TIME);
      alarm0_set_hour_12(!alarm0_get_hour_12());
      alarm0_sync_to_rtc();
      rom_write(ROM_ALARM0_IS12, alarm0_get_hour_12() ? 1 : 0);
      break;
  }
}


void sm_clock_mod_global_flag(unsigned char from, unsigned char to, enum task_events ev)
{
  CDBG("sm_clock_mod_global_flag %bd %bd %bd\n", from, to, ev);
  
  // 按mod1进入修改全局标志位模式
  if(get_sm_ss_state(to) == SM_CLOCK_MODIFY_GLOBAL_FLAG_INIT && ev == EV_KEY_MOD_LPRESS) {
    display_logo(DISPLAY_LOGO_TYPE_CLOCK, 3);
    return;
  }

  // 切换到修改全局标志位模式
  if(get_sm_ss_state(from) == SM_CLOCK_MODIFY_GLOBAL_FLAG_INIT 
    && get_sm_ss_state(to) == SM_CLOCK_MODIFY_GLOBAL_FLAG_PS
    && ev == EV_KEY_MOD_UP) {
    display_global_flag(IS_PS);
    return;
  }

  // set0 省电模式超时时间设置
  if(get_sm_ss_state(to) == SM_CLOCK_MODIFY_GLOBAL_FLAG_PS && ev == EV_KEY_SET_PRESS) {
    inc_write(IS_PS);
    display_global_flag(IS_PS);
    return;
  }
  
  // mod0 进入设置整点报时on/off状态
  if(get_sm_ss_state(to) == SM_CLOCK_MODIFY_GLOBAL_FLAG_BS && ev == EV_KEY_MOD_PRESS) {
    display_global_flag(IS_BS);
    return;
  }
  
  // set0 整点报时on/off
  if(get_sm_ss_state(to) == SM_CLOCK_MODIFY_GLOBAL_FLAG_BS && ev == EV_KEY_SET_PRESS) {
    inc_write(IS_BS);
    display_global_flag(IS_BS);
    return;
  }  
  
  // mod0 进入闹铃音乐选择
  if(get_sm_ss_state(to) == SM_CLOCK_MODIFY_GLOBAL_FLAG_ALARM_MUSIC && ev == EV_KEY_MOD_PRESS) {
    display_global_flag(IS_MUSIC);
    return;
  }

  // set0 闹铃音乐设置
  if(get_sm_ss_state(to) == SM_CLOCK_MODIFY_GLOBAL_FLAG_ALARM_MUSIC && ev == EV_KEY_SET_PRESS) {
    inc_write(IS_MUSIC);
    display_global_flag(IS_MUSIC);
    beeper_play_music();
    return;
  }
  
  // mod0 进入按键音打开关闭
  if(get_sm_ss_state(to) == SM_CLOCK_MODIFY_GLOBAL_FLAG_BEEP && ev == EV_KEY_MOD_PRESS) {
    display_global_flag(IS_BEEP);
    return;
  } 

  // set0 按键音打开关闭
  if(get_sm_ss_state(to) == SM_CLOCK_MODIFY_GLOBAL_FLAG_BEEP && ev == EV_KEY_SET_PRESS) {
    inc_write(IS_BEEP);
    display_global_flag(IS_BEEP);
    return;
  }  
  
  // mod0 进入1224小时设置状态
  if(get_sm_ss_state(to) == SM_CLOCK_MODIFY_GLOBAL_FLAG_1224 && ev == EV_KEY_MOD_PRESS) {
    display_global_flag(IS_1224);
    return;
  }
  
  // set0 1224模式切换
  if(get_sm_ss_state(to) == SM_CLOCK_MODIFY_GLOBAL_FLAG_1224 && ev == EV_KEY_SET_PRESS) {
    inc_write(IS_1224);
    display_global_flag(IS_1224);
    return;
  }
  
  // mod0 进入设置省电模式状态
  if(get_sm_ss_state(to) == SM_CLOCK_MODIFY_GLOBAL_FLAG_PS && ev == EV_KEY_MOD_PRESS) {
    display_global_flag(IS_PS);
    return;
  }
}