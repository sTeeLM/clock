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
