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

const char * code sm_global_flag_mod_ss_name[] = 
{
  "SM_GLOBAL_FLAG_MODIFY_INIT",
  "SM_GLOBAL_FLAG_MODIFY_PS",
  "SM_GLOBAL_FLAG_MODIFY_BEEP",
	"SM_GLOBAL_FLAG_MODIFY_MUSIC_TO",
  "SM_GLOBAL_FLAG_MODIFY_1224",
  NULL
};

static void display_global_flag(unsigned char what)
{

  led_clear();
  
  CDBG("display_global_flag %bu\n", what);
  
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
		case IS_MUSIC_TO:
      led_set_code(5, 'B');
      led_set_code(4, 'T');
      led_set_code(3, 'O');
			led_set_code(2, LED_CODE_BLACK);
      led_set_code(1, (beeper_get_music_to() / 10) + 0x30);
      led_set_code(0, (beeper_get_music_to() % 10) + 0x30);
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
  }
}

static void inc_write(unsigned char what)
{
	bit is12;
  switch(what) {
    case IS_PS:
      power_inc_powersave_to();
      rom_write(ROM_POWERSAVE_TO, power_get_powersave_to());
      break;
    case IS_BEEP:
      beeper_set_beep_enable(!beeper_get_beep_enable());
			rom_write(ROM_BEEPER_ENABLE, beeper_get_beep_enable() ? 1 : 0);
      break;
		case IS_MUSIC_TO:
			beeper_inc_music_to();
			rom_write(ROM_BEEPER_MUSIC_TO, beeper_get_music_to());
			break;
    case IS_1224:
			is12 = rom_read(ROM_TIME_IS12);
      clock_set_hour_12(!is12);
      clock_sync_to_rtc(CLOCK_SYNC_TIME);
      alarm0_set_hour_12(!is12);
      lt_timer_set_hour_12(!is12);
      rom_write(ROM_TIME_IS12, !is12);
      break;
  }
}

void sm_global_flag_mod_init(unsigned char from, unsigned char to, enum task_events ev)
{
  CDBG("sm_global_flag_mod_init %bu %bu %bu\n", from, to, ev);
	lt_timer_switch_off();
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
  CDBG("sm_global_flag_mod_submod0 %bu %bu %bu\n", from, to, ev);
	sm_global_flag_mod(IS_PS, ev);
}

void sm_global_flag_mod_submod1(unsigned char from, unsigned char to, enum task_events ev)
{
  CDBG("sm_global_flag_mod_submod1 %bu %bu %bu\n", from, to, ev);
	sm_global_flag_mod(IS_BEEP, ev);
}

void sm_global_flag_mod_submod2(unsigned char from, unsigned char to, enum task_events ev)
{
  CDBG("sm_global_flag_mod_submod2 %bu %bu %bu\n", from, to, ev);
	sm_global_flag_mod(IS_MUSIC_TO, ev);
}

void sm_global_flag_mod_submod3(unsigned char from, unsigned char to, enum task_events ev)
{
  CDBG("sm_global_flag_mod_submod3 %bu %bu %bu\n", from, to, ev);
	sm_global_flag_mod(IS_1224, ev);
}

