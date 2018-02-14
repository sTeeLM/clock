#include <STC89C5xRC.H>
#include "fuse.h"
#include "hg.h"
#include "gyro.h"
#include "task.h"
#include "debug.h"
#include "serial_hub.h"
#include "sm.h"
#include "power.h"

#define FUSE0_BROKE_MASK 0x1
#define FUSE1_BROKE_MASK 0x2

static bit fuse_enabled;

sbit POWER_5V_EN  = P3 ^ 6;

static void fuse_power_on(void)
{
  CDBG("fuse_power_on\n");
  
  serial_set_ctl_bit(SERIAL_BIT_FUSE_EN, 0);
  serial_ctl_out();
}
  
static void fuse_power_off(void)
{
  CDBG("fuse_power_off\n");
  serial_set_ctl_bit(SERIAL_BIT_FUSE_EN, 1);
  serial_ctl_out();
}

void fuse_initialize (void)
{
  CDBG("fuse_initialize\n");
  fuse_enabled = 0;

  POWER_5V_EN = 1;
}


void fuse_trigger(bit enable)
{
  CDBG("fuse_trigger %bd\n", enable ? 1 : 0);
  // 升压
  POWER_5V_EN = !enable;
  
  // trigger
  serial_set_ctl_bit(SERIAL_BIT_FUSE0_TRIGGER, !enable);
  serial_set_ctl_bit(SERIAL_BIT_FUSE1_TRIGGER, !enable);
  serial_ctl_out();
}

void fuse_enter_powersave(void)
{
  CDBG("fuse_enter_powersave\n");
}

void fuse_leave_powersave(void)
{
  CDBG("fuse_leave_powersave\n");
}

void scan_fuse(unsigned int status)
{
  bit has_event = 0;
  CDBG("scan_fuse %x\n", status);
  
  if((status & FUSE0_BROKE_MASK) == 0) {
    CDBG("EV_FUSE0_BROKE\n");
    set_task(EV_FUSE0_BROKE);
    has_event = 1;
  }
  
  if((status & FUSE1_BROKE_MASK) == 0) {
    CDBG("EV_FUSE1_BROKE\n");
    set_task(EV_FUSE1_BROKE);
    has_event = 1;
  }

  if(has_event == 1 && power_test_flag()) {
    power_clr_flag();
  }
}

void fuse_proc(enum task_events ev)
{
  CDBG("fuse_proc\n");
  run_state_machine(ev);
}


void fuse_set_fuse_broke(unsigned char index, bit enable)
{
  CDBG("fuse_set_fuse_broke %bd %bd\n", index, enable ? 1 : 0);
  
  if(!fuse_enabled) return;
  
  if(index == 0)
    serial_set_ctl_bit(SERIAL_BIT_FUSE0_BROKE_TEST, !enable);
  else
    serial_set_ctl_bit(SERIAL_BIT_FUSE1_BROKE_TEST, !enable);

  serial_ctl_out();
}

void fuse_enable(bit enable)
{
  CDBG("fuse_enable %bd\n", enable ? 1 : 0);
  
  if(enable && !fuse_enabled)
    fuse_power_on();
  else if(!enable && fuse_enabled)
    fuse_power_off();
  
  fuse_enabled = enable;
}