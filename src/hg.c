#include "hg.h"
#include "debug.h"
#include "task.h"
#include "serial_hub.h"
#include "sm.h"
#include "power.h"
#include "int_hub.h"
#include "misc.h"

static bit hg_enabled;
static unsigned char hg_state; // 只有低四位有效


void hg_enter_powersave(void)
{
  CDBG(("hg_enter_powersave\n"));
}

void hg_leave_powersave(void)
{
  CDBG(("hg_leave_powersave\n"));
}

static unsigned char _hg_get_state(unsigned int status)
{
  return ((status & 0x3C) >> 2) & 0xF;
}

void scan_hg(unsigned int status)
{
  unsigned char hg_new_state;
  CDBG(("scan_hg 0x%04x\n", status));
  
  hg_new_state = _hg_get_state(status);
  
  CDBG(("scan_hg hg_state = %02bx, hg_new_state = %02bx\n", hg_state, hg_new_state));
  
  if(hg_new_state != hg_state) {
    if(hg_enabled) {
      CDBG(("EV_ROTATE_HG!\n"));
      set_task(EV_ROTATE_HG);
    }
    if(power_test_flag()) {
      power_clr_flag();
    }
    
    CDBG(("hg_state: 0x%02bx->0x%02bx\n", hg_state, hg_new_state));
    hg_state = hg_new_state;
  }
  
}


static void hg_power_on(void)
{
	unsigned int status;

  serial_set_ctl_bit(SERIAL_BIT_HG_EN, 0);  
  serial_ctl_out();
	
	delay_ms(2);
	
	status = int_hub_get_status();
	hg_state = _hg_get_state(status);
	
	hg_enabled = 1;
}

static void hg_power_off(void)
{
  serial_set_ctl_bit(SERIAL_BIT_HG_EN, 1);  
  serial_ctl_out();
  hg_enabled = 0;
}

void hg_initialize (void)
{
  CDBG(("hg_initialize\n"));
  hg_power_off();
}

void hg_enable(bit enable)
{
  CDBG(("hg_enable %bu\n", enable ? 1 : 0));
  if(enable && !hg_enabled) {
    hg_power_on();
    // hg_enabled = 1 set by hg_cb_set_enable
  } else if(!enable && hg_enabled){
    hg_power_off();
  }
}

bit hg_get_enable(void)
{
	return hg_enabled;
}

unsigned char hg_get_state(void)
{
  CDBG(("hg_get_state return %02bx\n", hg_state));
  return hg_state;
}

