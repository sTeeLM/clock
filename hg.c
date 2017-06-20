#include "hg.h"
#include "debug.h"
#include "task.h"
#include "serial_hub.h"
#include "sm.h"

static unsigned char hg_state;

void hg_initialize (void)
{
  CDBG("hg_initialize\n");
	hg_state = 0;
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
      set_task(EV_ROTATE_HG);
			hg_state = 0;
			hg_state = !serial_test_state_bit(SERIAL_BIT_HG3_HIT);
			hg_state = (hg_state << 1) | !serial_test_state_bit(SERIAL_BIT_HG2_HIT);
			hg_state = (hg_state << 1) | !serial_test_state_bit(SERIAL_BIT_HG1_HIT);
			hg_state = (hg_state << 1) | !serial_test_state_bit(SERIAL_BIT_HG0_HIT);			
      hg_fix();
      serial_state_in();
    }
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

