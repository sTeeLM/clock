#include "sm_power_pack_powersave.h"
#include "power.h"
#include "debug.h"

const char * code sm_power_pack_powersave_ss_name[] = {
  "SM_POWER_PACK_POWERSAVE_INIT",
  "SM_POWER_PACK_POWERSAVE_SLEEP",
  NULL
};

void sm_power_pack_powersave_init(unsigned char from, unsigned char to, enum task_events ev)
{
  CDBG("sm_power_pack_powersave_init %bu %bu %bu\n", from, to, ev);
}

void sm_power_pack_powersave_submod0(unsigned char from, unsigned char to, enum task_events ev)
{
  CDBG("sm_power_pack_powersave_submod0 %bu %bu %bu\n", from, to, ev);
  // 该睡眠了
  if(ev == EV_250MS) {
    power_enter_powersave();
    power_leave_powersave();
    return;
  } 
}
