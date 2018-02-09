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
static bit fuse0_shorted;
static bit fuse0_broked;
static bit fuse1_shorted;
static bit fuse1_broked;

sbit POWER_5V_EN  = P3 ^ 6;

static void fuse_power_on(void)
{
  CDBG("fuse_power_on\n");

  fuse0_broked  = 0;
  fuse1_broked  = 0;  
  
  serial_set_ctl_bit(SERIAL_BIT_FUSE_EN, 0);
  serial_ctl_out();
}
  
static void fuse_power_off(void)
{
  CDBG("fuse_power_off\n");
  serial_set_ctl_bit(SERIAL_BIT_FUSE_EN, 1);
  serial_ctl_out();
  fuse0_broked  = 0;
  fuse1_broked  = 0;
}

void fuse_initialize (void)
{
  CDBG("fuse_initialize\n");
  fuse_enabled = 0;
  
  fuse0_broked  = 0;
  fuse1_broked  = 0;
  POWER_5V_EN = 1;
}


bit fuse_test_broke(unsigned char index)
{
  if(index == 0) {
    return fuse0_broked;
  } else {
    return fuse1_broked;
  }
}

void fuse_trigger(bit enable)
{

  // ษýัน
  POWER_5V_EN = !enable;
  
  // trigger
  if(!fuse0_broked) {
    serial_set_ctl_bit(SERIAL_BIT_FUSE0_TRIGGER, !enable);
  }
  if(!fuse1_broked) {
    serial_set_ctl_bit(SERIAL_BIT_FUSE1_TRIGGER, !enable);
  }
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
    fuse0_broked = 1;
    has_event = 1;
  } else {
    fuse0_broked = 0;
  }
  
  if((status & FUSE1_BROKE_MASK) == 0) {
    CDBG("EV_FUSE1_BROKE\n");
    set_task(EV_FUSE1_BROKE);
    fuse1_broked = 1;
    has_event = 1;
  } else {
    fuse1_broked = 0;
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