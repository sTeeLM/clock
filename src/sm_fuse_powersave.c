#include "sm_fuse_powersave.h"
#include "debug.h"
#include "power.h"
#include "fuse.h"
#include "tripwire.h"
#include "thermo.h"
#include "gyro.h"
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
  tripwire_enable(0);
  thermo_hi_enable(0);
  thermo_lo_enable(0);
  gyro_enable(0);
  hg_enable(0);
}

void sm_fuse_powersave(unsigned char from, unsigned char to, enum task_events ev)
{
  CDBG("sm_fuse_powersave %bd %bd %bd\n", from, to, ev);
  // �ý���ڵ�ģʽ��
  if(get_sm_ss_state(to) == SM_FUSE_POWERSAVE_PS && ev == EV_250MS) {
    // ����˯��
    power_enter_powersave();
    power_leave_powersave();
    return;
  }

  if(get_sm_ss_state(to) == SM_FUSE_POWERSAVE_PREDETONATE) {
    stop_peripheral(); // �ر�������Χ��·���Լ�lt_timer
    lt_timer_reset();
    set_task(EV_FUSE_SEL0);
  }
}