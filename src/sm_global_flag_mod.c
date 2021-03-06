#include "sm_global_flag_mod.h"
#include "mod_common.h"
#include "led.h"
#include "beeper.h"
#include "power.h"
#include "clock.h"
#include "alarm.h"
#include "debug.h"
#include "rom.h"
#include "lt_timer.h"
#include "remote.h"
#include "cext.h"

#ifdef __CLOCK_DEBUG__
const char * code sm_global_flag_mod_ss_name[] = 
{
  "SM_GLOBAL_FLAG_MODIFY_INIT",
  "SM_GLOBAL_FLAG_MODIFY_PS",
  "SM_GLOBAL_FLAG_MODIFY_AUTOLIGHT",	
  "SM_GLOBAL_FLAG_MODIFY_BEEP",
  "SM_GLOBAL_FLAG_MODIFY_1224",
  "SM_GLOBAL_FLAG_MODIFY_REMOTE",
  NULL
};
#endif

static void display_global_flag(unsigned char what)
{

  led_clear();
  
  CDBG(("display_global_flag %bu\n", what));
  
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
		case IS_AUTOLIGHT:
      led_set_code(5, 'A');
      led_set_code(4, 'U');
      led_set_code(3, 'L');
      if(led_get_auto_light_enable()) {
        led_set_code(2, LED_CODE_BLACK);
        led_set_code(1, 'O');
        led_set_code(0, 'N');        
      } else {
        led_set_code(2, 'O');
        led_set_code(1, 'F');
        led_set_code(0, 'F');
      }
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
      if(!rom_read(ROM_TIME_IS12)) {
        led_set_code(1, '2');
        led_set_code(0, '4');
      } else {
        led_set_code(1, '1');
        led_set_code(0, '2');
      }
      break;
    case IS_REMOTE:
      led_set_code(5, 'R');
      led_set_code(4, 'A');
      led_set_code(3, LED_CODE_BLACK);
      if(remote_get_enable()) {
        led_set_code(2, LED_CODE_BLACK);
        led_set_code(1, 'O');
        led_set_code(0, 'N');        
      } else {
        led_set_code(2, 'O');
        led_set_code(1, 'F');
        led_set_code(0, 'F');
      }
      break;
  }
}

static void inc_write(unsigned char what)
{
  bit is12;
  switch(what) {
    case IS_PS:
      power_inc_powersave_to();
      power_write_rom_powersave_to();
      break;
		case IS_AUTOLIGHT:
			led_set_auto_light_enable(!led_get_auto_light_enable());
			led_write_rom_auto_light_enable();
			break;
    case IS_BEEP:
      beeper_set_beep_enable(!beeper_get_beep_enable());
      beeper_write_rom_beeper_enable();
      break;
    case IS_1224:
      is12 = rom_read(ROM_TIME_IS12);
      clock_set_hour_12(!is12);
      clock_sync_to_rtc(CLOCK_SYNC_TIME);
      alarm0_set_hour_12(!is12);
      lt_timer_set_hour_12(!is12);
      rom_write(ROM_TIME_IS12, !is12);
      break;
    case IS_REMOTE:
      remote_enable(!remote_get_enable());
      remote_write_rom_remote_enable();
      break;
  }
}

void sm_global_flag_mod_init(unsigned char from, unsigned char to, enum task_events ev)
{
#ifdef __CLOCK_DEBUG__
  CDBG(("sm_global_flag_mod_init %bu %bu %bu\n", from, to, ev));
#else
  UNUSED_PARAM(from);
  UNUSED_PARAM(to);
  UNUSED_PARAM(ev);
#endif  
  lt_timer_switch_off();
	led_test_set_auto_light_enable();
  display_logo(DISPLAY_LOGO_TYPE_GLOBAL_FLAG, 0);
}

static void sm_global_flag_mod(unsigned char what,  enum task_events ev)
{
  if(ev == EV_KEY_MOD_UP || ev == EV_KEY_MOD_PRESS) {
    display_global_flag(what);
    return;
  }
  
  if(ev == EV_KEY_SET_PRESS) {
    inc_write(what);
    display_global_flag(what);
    return;
  }
}

void sm_global_flag_mod_submod0(unsigned char from, unsigned char to, enum task_events ev)
{
  UNUSED_PARAM(from);
  UNUSED_PARAM(to);
  
  sm_global_flag_mod(IS_PS, ev);
}

void sm_global_flag_mod_submod1(unsigned char from, unsigned char to, enum task_events ev)
{
  UNUSED_PARAM(from);
  UNUSED_PARAM(to);
  
  sm_global_flag_mod(IS_AUTOLIGHT, ev);
}

void sm_global_flag_mod_submod2(unsigned char from, unsigned char to, enum task_events ev)
{
  UNUSED_PARAM(from);
  UNUSED_PARAM(to);
  
  sm_global_flag_mod(IS_BEEP, ev);
}

void sm_global_flag_mod_submod3(unsigned char from, unsigned char to, enum task_events ev)
{
  UNUSED_PARAM(from);
  UNUSED_PARAM(to);
  
  sm_global_flag_mod(IS_1224, ev);
}

void sm_global_flag_mod_submod4(unsigned char from, unsigned char to, enum task_events ev)
{
  UNUSED_PARAM(from);
  UNUSED_PARAM(to);
  
  sm_global_flag_mod(IS_REMOTE, ev);
}


