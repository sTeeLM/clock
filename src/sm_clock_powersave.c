#include "sm_clock_powersave.h"
#include "debug.h"
#include "led.h"
#include "power.h"
#include "clock.h"
#include "cext.h"
#include "mod_common.h"
#include "timer.h"

const char * code sm_clock_powersave_ss_name[] = 
{
  "SM_CLOCK_POWERSAVE_INIT",
  "SM_CLOCK_POWERSAVE_PS",
  NULL
};

void sm_clock_powersave(unsigned char from, unsigned char to, enum task_events ev)
{
  CDBG("sm_clock_powersave %bu %bu %bu\n", from, to, ev);

  // 该进入节电模式了
  if(get_sm_ss_state(to) == SM_CLOCK_POWERSAVE_PS && ev == EV_250MS) {
    // 进入睡眠
    power_enter_powersave();
    power_leave_powersave();
    return;
  } 
  
}
