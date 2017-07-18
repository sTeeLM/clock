#include "tripwire.h"
#include "debug.h"
#include "serial_hub.h"
#include "sm.h"
#include "power.h"

#define TRIPWIRE_HIT_MASK 0x1000

static bit tripwire_enabled;

static void tripwire_power_on(void)
{
  CDBG("tripwire_power_on\n");
	serial_set_ctl_bit(SERIAL_BIT_TRIPWIRE_EN, 1);
	serial_ctl_out();
}

static void tripwire_power_off(void)
{
  CDBG("tripwire_power_off\n");
	serial_set_ctl_bit(SERIAL_BIT_TRIPWIRE_EN, 0);
	serial_ctl_out();
}

void tripwire_initialize (void)
{
  CDBG("tripwire_initialize\n");
  tripwire_enabled = 0;
}

void tripwire_enter_powersave(void)
{
  CDBG("tripwire_enter_powersave\n");
}

void tripwire_leave_powersave(void)
{
  CDBG("tripwire_leave_powersave\n");
}

void scan_tripwire(unsigned int status)
{
  CDBG("scan_tripwire %x\n", status);
  
  if((TRIPWIRE_HIT_MASK & status) == 0) {
    set_task(EV_TRIPWIRE);
    if(power_test_flag()) {
      power_clr_flag();
    }
  }
}

void tripwire_proc(enum task_events ev)
{
  CDBG("tripwire_proc\n");
  run_state_machine(ev);
}

void tripwire_enable(bit enable)
{
	CDBG("tripwire_enable %bd\n", enable ? 1 : 0);
  if(enable && !tripwire_enabled) {
    tripwire_power_on();
  } else if(!enable && tripwire_enabled){
    tripwire_power_off();
  }
  tripwire_enabled = enable;
}

void tripwire_set_broke(bit broke)
{
	CDBG("tripwire_set_broke %bd\n", broke ? 1: 0);
  if(!tripwire_enabled) return;
	serial_set_ctl_bit(SERIAL_BIT_TRIPWIRE_TEST, broke);
	serial_ctl_out();
}