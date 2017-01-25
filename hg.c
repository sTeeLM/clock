#include "hg.h"
#include "debug.h"
#include "task.h"
#include "serial_hub.h"
#include "sm.h"

void hg_initialize (void)
{
  CDBG("hg_initialize\n");
}

static void hg_fix(void)
{
  if(!serial_test_state_bit(SERIAL_BIT_HG0_HIT)) {
    serial_set_ctl_bit(SERIAL_BIT_HG0_FIX, !serial_test_ctl_bit(SERIAL_BIT_HG0_FIX));
  }
  if(!serial_test_state_bit(SERIAL_BIT_HG1_HIT)) {
    serial_set_ctl_bit(SERIAL_BIT_HG1_FIX, !serial_test_ctl_bit(SERIAL_BIT_HG1_FIX));
  }
  if(!serial_test_state_bit(SERIAL_BIT_HG2_HIT)) {
    serial_set_ctl_bit(SERIAL_BIT_HG2_FIX, !serial_test_ctl_bit(SERIAL_BIT_HG2_FIX));
  }
  if(!serial_test_state_bit(SERIAL_BIT_HG3_HIT)) {
    serial_set_ctl_bit(SERIAL_BIT_HG3_FIX, !serial_test_ctl_bit(SERIAL_BIT_HG3_FIX));
  }
  serial_ctl_out();
}

void scan_hg(void)
{
  CDBG("scan_hg\n");
  if(!serial_test_state_bit(SERIAL_BIT_HG0_HIT)
    ||!serial_test_state_bit(SERIAL_BIT_HG1_HIT)
    ||!serial_test_state_bit(SERIAL_BIT_HG2_HIT)
    ||!serial_test_state_bit(SERIAL_BIT_HG3_HIT)) {
      set_task(EV_HG);
      hg_fix();
      serial_state_in();
    }
}

void hg_proc(enum task_events ev)
{
  CDBG("hg_proc\n");
  run_state_machine(ev);
}