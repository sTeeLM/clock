#include "thermo.h"
#include "debug.h"
#include "task.h"
#include "serial_hub.h"
#include "sm.h"

void thermo_initialize (void)
{
  CDBG("thermo_initialize\n");
}

void scan_thermo(void)
{
  CDBG("scan_thermo\n");
  if(!serial_test_state_bit(SERIAL_BIT_THERMO_LO_HIT))
  {
    set_task(EV_THERMO_LO);
  }
  
  if(!serial_test_state_bit(SERIAL_BIT_THERMO_HI_HIT))
  {
    set_task(EV_THERMO_HI);
  }
}

void thermo_proc(enum task_events ev)
{
  CDBG("thermo_proc\n");
  run_state_machine(ev);
}

