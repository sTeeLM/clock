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

void sm_fuse_detonate(unsigned char from, unsigned char to, enum task_events ev)
{
  CDBG("sm_fuse_detonate %bd %bd %bd\n", from, to, ev);
  
  // 该通电了
  if(get_sm_ss_state(to) == SM_FUSE_DETONATE_CHARGE && ev == EV_250MS) {
    display_detonate();
    fuse_trigger(1);
    common_state = 0;
    return;
  }
  
  // 记录通电时间，30S或者mod0停止
  if(get_sm_ss_state(to) == SM_FUSE_DETONATE_CHARGE && (ev == EV_1S || ev == EV_KEY_MOD_PRESS)) {
    common_state ++;
    if(common_state > MAX_FUSE_CHARGE_TIME) {
      fuse_trigger(0);
      fuse_enable(0);
      set_task(EV_FUSE_SEL0);
    }
    return;
  }
  
  
}