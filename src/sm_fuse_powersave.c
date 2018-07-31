#include "sm_fuse_powersave.h"
#include "debug.h"
#include "power.h"
#include "fuse.h"
#include "thermo.h"
#include "mpu.h"
#include "hg.h"
#include "lt_timer.h"
#include "remote.h"
#include "cext.h"

#ifdef __CLOCK_DEBUG__
const char * code sm_fuse_powersave_ss_name[] =
{
  "SM_FUSE_POWERSAVE_INIT",
  "SM_FUSE_POWERSAVE_PS",
  "SM_FUSE_POWERSAVE_PREDETONATE",
  "SM_FUSE_POWERSAVE_DISARM",
  NULL
};
#endif

static void roll_back(void)
{
  thermo_hi_enable(0);
  thermo_lo_enable(0);
  mpu_enable(0);
  hg_enable(0);
  remote_fuse_enable(0);
  lt_timer_reset();
}

void sm_fuse_powersave_init(unsigned char from, unsigned char to, enum task_events ev)
{
  UNUSED_PARAM(from);
  UNUSED_PARAM(to);
  UNUSED_PARAM(ev);
}

void sm_fuse_powersave_submod0(unsigned char from, unsigned char to, enum task_events ev)
{
  UNUSED_PARAM(from);
  UNUSED_PARAM(to);
  
  if(ev == EV_250MS) {
    // 进入睡眠
    power_enter_powersave();
    power_leave_powersave();
    return;
  }
}

void sm_fuse_powersave_submod1(unsigned char from, unsigned char to, enum task_events ev)
{
  UNUSED_PARAM(from);
  UNUSED_PARAM(to);
  UNUSED_PARAM(ev);
  
  roll_back(); // 关闭所有外围电路，以及lt_timer,准备触发
  set_task(EV_KEY_V0);
}

void sm_fuse_powersave_submod2(unsigned char from, unsigned char to, enum task_events ev)
{
  UNUSED_PARAM(from);
  UNUSED_PARAM(to);
  UNUSED_PARAM(ev);
  
  roll_back(); // 关闭所有外围电路，以及lt_timer,以及fuse, 准备解除
  fuse_enable(0);
  set_task(EV_KEY_V0);
}
