#include "sm_power_pack_display.h"
#include "debug.h"
#include "power.h"
#include "led.h"

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
	led_set_code(5, val / 10 + 0x30);
	led_set_code(4, val % 10 + 0x30);	
}

void sm_power_pack_display_init(unsigned char from, unsigned char to, enum task_events ev)
{
  CDBG("sm_power_pack_display_init %bd %bd %bd\n", from, to, ev);
	clock_display(0);
  display_logo(DISPLAY_LOGO_TYPE_POWER_PACK, 0);
}

void sm_power_pack_display_submod0(unsigned char from, unsigned char to, enum task_events ev)
{
  CDBG("sm_power_pack_display_submod0 %bd %bd %bd\n", from, to, ev);
	if(ev == EV_KEY_MOD_UP || ev == EV_KEY_SET_UP) {
		power_5v_enable(1); // 打开5v电源
		return;
	}
	
	if(ev == EV_1S) {
		display_power_volume(); // 显示电量
		return;
	}
	
}