#include "fuse.h"
#include "hg.h"
#include "gyro.h"
#include "task.h"
#include "debug.h"
#include "serial_hub.h"
#include "sm.h"

#define FUSE0_TRIGGERED_MASK 0x1
#define FUSE1_TRIGGERED_MASK 0x2
#define FUSE0_SHORT_MASK 0x4
#define FUSE0_BROKE_MASK 0x8
#define FUSE1_SHORT_MASK 0x10
#define FUSE1_BROKE_MASK 0x20

static void fuse_power_on(void)
{
  CDBG("fuse_power_on\n");
	serial_set_ctl_bit(SERIAL_BIT_FUSE_EN, 1);
	serial_ctl_out();
}
  
static void fuse_power_off(void)
{
  CDBG("fuse_power_off\n");
	serial_set_ctl_bit(SERIAL_BIT_FUSE_EN, 0);
	serial_ctl_out();
}

void fuse_initialize (void)
{
  CDBG("fuse_initialize\n");
  fuse_power_on();
  fuse_power_off();
}

void scan_fuse(unsigned int status)
{
  CDBG("scan_fuse %x\n", status);
  
  // 如果还没有trigger
  if((status & FUSE0_TRIGGERED_MASK) != 0) {
    if((status & FUSE0_SHORT_MASK) == 0) {
			CDBG("EV_FUSE0_SHORT\n");
      set_task(EV_FUSE0_SHORT);
    }
    if((status & FUSE0_BROKE_MASK) == 0) {
			CDBG("EV_FUSE0_BROKE\n");
      set_task(EV_FUSE0_BROKE);
    }
  }
  
  if((status & FUSE1_TRIGGERED_MASK) != 0) {
    if((status & FUSE1_SHORT_MASK) == 0) {
			CDBG("EV_FUSE1_SHORT\n");
      set_task(EV_FUSE1_SHORT);
    }
    if((status & FUSE1_BROKE_MASK) == 0) {
			CDBG("EV_FUSE1_BROKE\n");
      set_task(EV_FUSE1_BROKE);
    }
  }
}

void fuse_proc(enum task_events ev)
{
  CDBG("fuse_proc\n");
  run_state_machine(ev);
}

void fuse_set_fuse_short(unsigned char index, bit enable)
{
	CDBG("fuse_set_fuse_short %bd %bd\n", index, enable ? 1 : 0);
	if(index == 0)
		serial_set_ctl_bit(SERIAL_BIT_FUSE0_SHORT_TEST, enable);
	else
		serial_set_ctl_bit(SERIAL_BIT_FUSE1_SHORT_TEST, enable);
	
	serial_ctl_out();
}

void fuse_set_fuse_broke(unsigned char index, bit enable)
{
	CDBG("fuse_set_fuse_broke %bd %bd\n", index, enable ? 1 : 0);
	if(index == 0)
		serial_set_ctl_bit(SERIAL_BIT_FUSE0_BROKE_TEST, enable);
	else
		serial_set_ctl_bit(SERIAL_BIT_FUSE1_BROKE_TEST, enable);

	serial_ctl_out();
}

void fuse_enable(bit enable)
{
	CDBG("fuse_enable %bd\n", enable ? 1 : 0);
  if(enable)
    fuse_power_on();
  else
    fuse_power_off();
}