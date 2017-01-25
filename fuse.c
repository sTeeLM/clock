#include "fuse.h"
#include "hg.h"
#include "gyro.h"
#include "task.h"
#include "debug.h"
#include "serial_hub.h"
#include "sm.h"

void fuse_initialize (void)
{
  CDBG("fuse_initialize\n");

}

void scan_fuse(void)
{
  CDBG("scan_fuse\n");
  
  // 如果还没有trigger
  if(serial_test_state_bit(SERIAL_BIT_FUSE0_TRIGGERED)) {
    if(!serial_test_state_bit(SERIAL_BIT_FUSE0_BROKE)
      ||!serial_test_state_bit(SERIAL_BIT_FUSE0_SHORT)) {
        set_task(EV_FUSE0_SHORT_BROKE);
      }
  }
  
  if(serial_test_state_bit(SERIAL_BIT_FUSE1_TRIGGERED)) {
    if(!serial_test_state_bit(SERIAL_BIT_FUSE1_BROKE)
      ||!serial_test_state_bit(SERIAL_BIT_FUSE1_SHORT)) {
        set_task(EV_FUSE1_SHORT_BROKE);
      }
  }
}

void fuse_proc(enum task_events ev)
{
  CDBG("fuse_proc\n");
  run_state_machine(ev);
}
