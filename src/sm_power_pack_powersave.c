#include "sm_power_pack_powersave.h"
#include "power.h"
#include "debug.h"
#include "cext.h"

#ifdef __CLOCK_DEBUG__
const char * code sm_power_pack_powersave_ss_name[] = {
  "SM_POWER_PACK_POWERSAVE_INIT",
  "SM_POWER_PACK_POWERSAVE_SLEEP",
  NULL
};
#endif

void sm_power_pack_powersave_init(unsigned char from, unsigned char to, enum task_events ev)
{
#ifdef __CLOCK__DEBUG__
  CDBG(("sm_power_pack_powersave_init %bu %bu %bu\n", from, to, ev));
#else
  UNUSED_PARAM(from);
  UNUSED_PARAM(to);
  UNUSED_PARAM(ev);
#endif
}

void sm_power_pack_powersave_submod0(unsigned char from, unsigned char to, enum task_events ev)
{
#ifdef __CLOCK__DEBUG__
  CDBG(("sm_power_pack_powersave_submod0 %bu %bu %bu\n", from, to, ev));
#else
  UNUSED_PARAM(from);
  UNUSED_PARAM(to);
#endif
  // 该睡眠了
  if(ev == EV_250MS) {
    power_enter_powersave();
    power_leave_powersave();
    return;
  } 
}
