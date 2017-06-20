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
    if(!serial_test_state_bit(SERIAL_BIT_FUSE0_BROKE)) {
      set_task(EV_FUSE0_BROKE);
    }
    
    if(!serial_test_state_bit(SERIAL_BIT_FUSE0_SHORT)) {
      set_task(EV_FUSE0_SHORT);
    }
  }
  
  if(serial_test_state_bit(SERIAL_BIT_FUSE1_TRIGGERED)) {
    if(!serial_test_state_bit(SERIAL_BIT_FUSE1_BROKE)) {
      set_task(EV_FUSE1_BROKE);
    }
    
    if(!serial_test_state_bit(SERIAL_BIT_FUSE1_SHORT)) {
      set_task(EV_FUSE1_SHORT);
    }
  }
}

void fuse_proc(enum task_events ev)
{
  CDBG("fuse_proc\n");
  run_state_machine(ev);
}

void fuse_set_fuse_short(unsigned char index, bit enable)
{
	CDBG("fuse_set_fuse_short %bd %bd\n", index, enable);
	if(index == 0)
		serial_set_ctl_bit(SERIAL_BIT_FUSE0_SHORT_TEST, enable);
	else
		serial_set_ctl_bit(SERIAL_BIT_FUSE1_SHORT_TEST, enable);
	
	serial_ctl_out();
}

void fuse_set_fuse_broke(unsigned char index, bit enable)
{
	CDBG("fuse_set_fuse_broke %bd %bd\n", index, enable);
	if(index == 0)
		serial_set_ctl_bit(SERIAL_BIT_FUSE0_BROKE_TEST, enable);
	else
		serial_set_ctl_bit(SERIAL_BIT_FUSE1_BROKE_TEST, enable);

	serial_ctl_out();
}

void fuse_enable(bit enable)
{
	CDBG("fuse_enable %bd\n", enable);
	serial_set_ctl_bit(SERIAL_BIT_FUSE_EN, enable);
	serial_ctl_out();
}