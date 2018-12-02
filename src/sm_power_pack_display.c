#include "sm_power_pack_display.h"
#include "debug.h"
#include "power.h"
#include "led.h"
#include "clock.h"
#include "mod_common.h"
#include "power.h"
#include "alarm.h"
#include "indicator.h"
#include "cext.h"
#include "sm_power_pack_powersave.h"

#ifdef __CLOCK_DEBUG__
const char * code sm_power_pack_display_ss_name[] = {
  "SM_POWER_PACK_DISPLAY_INIT",
  "SM_POWER_PACK_DISPLAY_POWER",
  NULL
};
#endif

static void display_power_percent(void)
{
  unsigned int val;
  
  if(power_5v_get_enable()) { // display voltage
    val = power_get_voltage();
    led_clear();
    led_set_code(5, 'P');
    led_set_code(4, 'O');
    led_set_code(3, '-');
    led_set_dp(2);
  } else { // display percent
    val = power_get_percent();
    CDBG(("display_power_percent bat is %bu%%\n", (unsigned char)val));
    led_clear();
    led_set_code(5, 'P');
    led_set_code(4, 'E');
    led_set_code(3, '-');
  }
  led_set_code(2, val / 100 + 0x30);
  led_set_code(1, (val % 100) / 10 + 0x30);
  led_set_code(0, (val % 100) % 10 + 0x30);
}

void sm_power_pack_display_init(unsigned char from, unsigned char to, enum task_events ev)
{
#ifdef __CLOCK__DEBUG__
  CDBG(("sm_power_pack_display_init %bu %bu %bu\n", from, to, ev));
#else
  UNUSED_PARAM(from);
  UNUSED_PARAM(to);
  UNUSED_PARAM(ev);
#endif
  clock_display(0);
  alarm_switch_off();
  indicator_clr();
  display_logo(DISPLAY_LOGO_TYPE_POWER_PACK, 0);
}

void sm_power_pack_display_submod0(unsigned char from, unsigned char to, enum task_events ev)
{
#ifdef __CLOCK__DEBUG__
  CDBG(("sm_power_pack_display_submod0 %bu %bu %bu\n", from, to, ev));
#else
  UNUSED_PARAM(from);
  UNUSED_PARAM(to);
#endif
  if(ev == EV_KEY_MOD_UP || ev == EV_KEY_SET_UP) {
    power_reset_powersave_to();
    return;
  }
  
  if(ev == EV_KEY_MOD_PRESS || ev == EV_KEY_SET_PRESS) {
    power_reset_powersave_to();
    if(ev == EV_KEY_SET_PRESS && from != (SM_POWER_PACK_POWERSAVE<<4|SM_POWER_PACK_POWERSAVE_SLEEP)) { // 开关5v输出
      power_5v_enable(!power_5v_get_enable());
      display_power_percent();
      if(power_5v_get_enable()) {
        indicator_set(INDICATOR_COLOR_GREEN, INDICATOR_MODE_BLINK);
      } else {
        indicator_set(INDICATOR_COLOR_GREEN, INDICATOR_MODE_OFF);
      }
    }
    return;
  }
  
  if(ev == EV_KEY_SET_LPRESS) { // 关机！
    power_3_3v_enable(0);
    return;
  }
  
  if(ev == EV_1S) {
    display_power_percent(); // 显示电量
    power_test_powersave_to();
    return;
  }
  
}
