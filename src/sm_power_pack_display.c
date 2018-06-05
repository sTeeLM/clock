#include "sm_power_pack_display.h"
#include "debug.h"
#include "power.h"
#include "led.h"
#include "clock.h"
#include "mod_common.h"
#include "power.h"
#include "alarm.h"

const char * code sm_power_pack_display_ss_name[] = {
  "SM_POWER_PACK_DISPLAY_INIT",
  "SM_POWER_PACK_DISPLAY_POWER",
  NULL
};

static void display_power_volume(void)
{
  unsigned char val;
  val = power_get_volume();
  led_clear();
  led_set_code(5, 'P');
  led_set_code(4, 'O');
  led_set_code(3, '-');
  led_set_code(2, val / 100 + 0x30);
  led_set_code(1, (val % 100) / 10 + 0x30);
  led_set_code(0, (val % 100) % 10 + 0x30);
}

void sm_power_pack_display_init(unsigned char from, unsigned char to, enum task_events ev)
{
  CDBG("sm_power_pack_display_init %bu %bu %bu\n", from, to, ev);
  clock_display(0);
  alarm_switch_off();
  display_logo(DISPLAY_LOGO_TYPE_POWER_PACK, 0);
}

void sm_power_pack_display_submod0(unsigned char from, unsigned char to, enum task_events ev)
{
  CDBG("sm_power_pack_display_submod0 %bu %bu %bu\n", from, to, ev);
  if(ev == EV_KEY_MOD_UP || ev == EV_KEY_SET_UP) {
    power_5v_enable(1); // 打开5v电源
    power_reset_powersave_to();
    return;
  }
  
  if(ev == EV_KEY_MOD_PRESS || ev == EV_KEY_SET_PRESS) {
    power_reset_powersave_to();
    if(ev == EV_KEY_SET_PRESS) { // 开关5v输出
      power_5v_enable(!power_5v_get_enable());
    }
  }
  
  if(ev == EV_1S) {
    display_power_volume(); // 显示电量
    power_test_powersave_to();
    return;
  }
  
}
