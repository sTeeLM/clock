#include "tripwire.h"
#include "debug.h"
#include "serial_hub.h"
#include "sm.h"

void tripwire_initialize (void)
{
  CDBG("tripwire_initialize\n");
}

void scan_tripwire(void)
{
  CDBG("scan_tripwire\n");
  if(!serial_test_state_bit(SERIAL_BIT_TRIPWIRE_HIT)) {
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
	CDBG("tripwire_enable %bd\n", enable);
	serial_set_ctl_bit(SERIAL_BIT_TRIPWIRE_EN, enable);
	serial_ctl_out();
}

void tripwire_set_broke(bit broke)
{
	CDBG("tripwire_set_broke %bd\n", broke);
	serial_set_ctl_bit(SERIAL_BIT_TRIPWIRE_TEST, broke);
	serial_ctl_out();
}