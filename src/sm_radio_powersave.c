#include "sm_radio_powersave.h"
#include "power.h"
#include "debug.h"
#include "cext.h"

const char * code sm_radio_powersave_ss_name[] = {
  "SM_RADIO_POWERSAVE_INIT",
  "SM_RADIO_POWERSAVE_SLEEP",
  NULL
};

void sm_radio_powersave_init(unsigned char from, unsigned char to, enum task_events ev)
{
  UNUSED_PARAM(from);
  UNUSED_PARAM(to);
  UNUSED_PARAM(ev);
}

void sm_radio_powersave_submod0(unsigned char from, unsigned char to, enum task_events ev)
{
  UNUSED_PARAM(from);
  UNUSED_PARAM(to);

  // 该睡眠了
  if(ev == EV_250MS) {
    power_enter_powersave();
    power_leave_powersave();
    return;
  } 
}
