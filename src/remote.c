#include "remote.h"
#include "debug.h"
#include "sm.h"
#include "serial_hub.h"
#include "power.h"

#define REMOTE_ARM_MASK 0x200
#define REMOTE_DISARM_MASK 0x400
#define REMOTE_DETONATE_MASK 0x800

static bit remote_enabled;

static void remote_power_off(void)
{
  CDBG("remote_power_off\n");
  serial_set_ctl_bit(SERIAL_BIT_REMOTE_EN, 1);
  serial_ctl_out();
  remote_enabled = 0;
}

static void remote_power_on(void)
{
  CDBG("remote_power_on\n");
  serial_set_ctl_bit(SERIAL_BIT_REMOTE_EN, 0);
  serial_ctl_out();
  remote_enabled = 1;
}

void remote_initialize (void)
{
  CDBG("remote_initialize\n");
  remote_power_off();
}

void remote_enter_powersave(void)
{
  CDBG("remote_enter_powersave\n");
}

void remote_leave_powersave(void)
{
  CDBG("remote_leave_powersave\n");
}

void scan_remote(unsigned int status)
{
  bit has_event = 0;
  CDBG("scan_remote  0x%04x\n", status);
  if((status & REMOTE_ARM_MASK) == 0) {
    if(remote_enabled) {
      CDBG("EV_REMOTE_ARM\n");
      set_task(EV_REMOTE_ARM);
    }
    has_event = 1;
  }
  
  if((status & REMOTE_DISARM_MASK) == 0) {
    if(remote_enabled) {
      CDBG("EV_REMOTE_DISARM\n");
      set_task(EV_REMOTE_DISARM);
    }
    has_event = 1;
  }
  
  if((status & REMOTE_DETONATE_MASK) == 0) {
    if(remote_enabled) {
      CDBG("EV_REMOTE_DETONATE\n");
      set_task(EV_REMOTE_DETONATE);
    }
    has_event = 1;
  }
  
  if(has_event == 1 && power_test_flag()) {
    power_clr_flag();
  }
}

void remote_proc(enum task_events ev)
{
  CDBG("remote_proc %bu\n", ev);
  run_state_machine(ev);
}

void remote_enable(bit enable)
{
  CDBG("remote_enable is %bu\n", enable ? 1:0);
  if(enable && !remote_enabled) {
    remote_power_on();
  } else if(!enable && remote_enabled) {
    remote_power_off();
  }
}
