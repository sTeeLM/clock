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

void thermo_hi_enable(bit enable)
{
	CDBG("thermo_hi_enable %bd\n", enable);
	serial_set_ctl_bit(SERIAL_BIT_THERMO_HI_EN, enable);
	serial_ctl_out();
}

void thermo_lo_enable(bit enable)
{
	CDBG("thermo_lo_enable %bd\n", enable);
	serial_set_ctl_bit(SERIAL_BIT_THERMO_LO_EN, enable);
	serial_ctl_out();
}

void thermo_hi_set_hit(bit hit)
{
	CDBG("thermo_hi_set_hit %bd\n", hit);
	serial_set_ctl_bit(SERIAL_BIT_THERMO_HI_TEST, hit);
	serial_ctl_out();
}

void thermo_lo_set_hit(bit hit)
{
	CDBG("thermo_lo_set_hit %bd\n", hit);
	serial_set_ctl_bit(SERIAL_BIT_THERMO_LO_TEST, hit);
	serial_ctl_out();
}
