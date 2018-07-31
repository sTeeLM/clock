#include "sm_clock_powersave.h"
#include "debug.h"
#include "led.h"
#include "power.h"
#include "clock.h"
#include "cext.h"
#include "mod_common.h"
#include "timer.h"

#ifdef __CLOCK_DEBUG__
const char * code sm_clock_powersave_ss_name[] = 
{
  "SM_CLOCK_POWERSAVE_INIT",
  "SM_CLOCK_POWERSAVE_PS",
  NULL
};
#endif

void sm_clock_powersave(unsigned char from, unsigned char to, enum task_events ev)
{
  UNUSED_PARAM(from); 
  
  // 该进入节电模式了
  if(get_sm_ss_state(to) == SM_CLOCK_POWERSAVE_PS && ev == EV_250MS) {
    // 进入睡眠
    power_enter_powersave();
    power_leave_powersave();
    return;
  } 
  
}
