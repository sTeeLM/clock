#include "tripwire.h"
#include "debug.h"
#include "serial_hub.h"
#include "sm.h"

#define TRIPWIRE_HIT_MASK 0x1000

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
  tripwire_power_on();
  tripwire_power_off();
}

void scan_tripwire(unsigned int status)
{
  CDBG("scan_tripwire %x\n", status);
  if((TRIPWIRE_HIT_MASK & status) == 0) {
    set_task(EV_TRIPWIRE);
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
  if(enable) {
    tripwire_power_on();
  } else {
    tripwire_power_off();
  }
}

void tripwire_set_broke(bit broke)
{
	CDBG("tripwire_set_broke %bd\n", broke ? 1: 0);
	serial_set_ctl_bit(SERIAL_BIT_TRIPWIRE_TEST, broke);
	serial_ctl_out();
}