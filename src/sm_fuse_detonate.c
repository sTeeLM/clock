#include "sm_fuse_detonate.h"
#include "debug.h"
#include "rom.h"
#include "mod_common.h"
#include "led.h"
#include "fuse.h"

#define MAX_FUSE_CHARGE_TIME 30 //s

const char * code sm_fuse_detonate_ss_name[] = 
{
  "SM_FUSE_DETONATE_INIT",
  "SM_FUSE_DETONATE_CHARGE",
  NULL
};

static void display_detonate(void)
{
  unsigned char i;
  led_clear();
  for(i = 0; i<6; i++) {
    led_set_code(i, '0');
    led_set_blink(i);
  }
}

void sm_fuse_detonate_init(unsigned char from, unsigned char to, enum task_events ev)
{
  CDBG("sm_fuse_detonate_init %bu %bu %bu\n", from, to, ev);
  display_detonate();
}


void sm_fuse_detonate_submod0(unsigned char from, unsigned char to, enum task_events ev)
{
  CDBG("sm_fuse_detonate_submod0 %bu %bu %bu\n", from, to, ev);
	
	if(ev == EV_250MS) {
		fuse_trigger(1);
		common_state = 0;
		return;
	}
	
	if(ev == EV_1S || ev == EV_KEY_MOD_PRESS || ev == EV_KEY_SET_PRESS) {
		if(ev == EV_1S)
			common_state ++;
		else
			common_state = MAX_FUSE_CHARGE_TIME + 1;
		if(common_state > MAX_FUSE_CHARGE_TIME) {
      fuse_trigger(0);
      fuse_enable(0);
			set_task(EV_KEY_V0);
		}
		return;
	}
}
