#include "sm_fuse_powersave.h"
#include "debug.h"
#include "power.h"
#include "fuse.h"
#include "thermo.h"
#include "mpu.h"
#include "hg.h"
#include "lt_timer.h"

const char * code sm_fuse_powersave_ss_name[] =
{
  "SM_FUSE_POWERSAVE_INIT",
  "SM_FUSE_POWERSAVE_PS",
  "SM_FUSE_POWERSAVE_PREDETONATE",
  NULL
};

static void stop_peripheral(void)
{
  thermo_enable(0);
  mpu_enable(0);
  hg_enable(0);
}

void sm_fuse_powersave(unsigned char from, unsigned char to, enum task_events ev)
{
  CDBG("sm_fuse_powersave %bd %bd %bd\n", from, to, ev);
  // 该进入节电模式了
  if(get_sm_ss_state(to) == SM_FUSE_POWERSAVE_PS && ev == EV_250MS) {
    // 进入睡眠
    power_enter_powersave();
    power_leave_powersave();
    return;
  }

  if(get_sm_ss_state(to) == SM_FUSE_POWERSAVE_PREDETONATE) {
    stop_peripheral(); // 关闭所有外围电路，以及lt_timer
    lt_timer_reset();
    set_task(EV_FUSE_SEL0);
  }
}