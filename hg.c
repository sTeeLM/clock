#include "hg.h"
#include "debug.h"
#include "task.h"
#include "serial_hub.h"
#include "sm.h"

static unsigned char hg_state;

#define HG0_HIT_MASK 0x100
#define HG1_HIT_MASK 0x200
#define HG2_HIT_MASK 0x400
#define HG3_HIT_MASK 0x800

void hg_initialize (void)
{
  CDBG("hg_initialize\n");
	hg_state = 0;
}

static void hg_fix(void)
{
  CDBG("hg_fix\n");
  serial_set_ctl_bit(SERIAL_BIT_HG0_FIX, (hg_state & 1) != 0);
  serial_set_ctl_bit(SERIAL_BIT_HG0_FIX, (hg_state & 2) != 0);  
  serial_set_ctl_bit(SERIAL_BIT_HG0_FIX, (hg_state & 4) != 0);
  serial_set_ctl_bit(SERIAL_BIT_HG0_FIX, (hg_state & 8) != 0);  
  serial_ctl_out();
}

void scan_hg(unsigned int status)
{
  CDBG("scan_hg %x\n", status);
  
  if((HG0_HIT_MASK & status) == 0
    || (HG1_HIT_MASK & status) == 0 
    || (HG2_HIT_MASK & status) == 0
    || (HG3_HIT_MASK & status) == 0) {
      set_task(EV_ROTATE_HG);
      hg_state = 0;
			hg_state = (status & 0x0F00) >> 8;
      hg_state = ~hg_state;	
      hg_fix();
    }
  CDBG("scan_hg hg_state = %bx\n", hg_state);
}


void hg_enable(bit enable)
{
	CDBG("hg_enable %bd\n", enable);
	serial_set_ctl_bit(SERIAL_BIT_HG_EN, enable);
	serial_ctl_out();
}

unsigned char hg_get_state(void)
{
	CDBG("hg_get_state\n");
	return hg_state;
}

