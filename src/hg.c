#include "hg.h"
#include "debug.h"
#include "task.h"
#include "serial_hub.h"
#include "sm.h"
#include "power.h"

static bit hg_enabled;
static unsigned char hg_state;

#define HG0_HIT_MASK 0x100
#define HG1_HIT_MASK 0x200
#define HG2_HIT_MASK 0x400
#define HG3_HIT_MASK 0x800

static void hg_power_on(void)
{
  CDBG("hg_power_on\n");
  hg_state = 0;
  serial_set_ctl_bit(SERIAL_BIT_HG_EN, 0);
  serial_ctl_out();
}

static void hg_power_off(void)
{
  CDBG("hg_power_off\n");
  serial_set_ctl_bit(SERIAL_BIT_HG_EN, 1);
  serial_ctl_out();
}

void hg_initialize (void)
{
  CDBG("hg_initialize\n");
  hg_enabled = 0;
}

void hg_enter_powersave(void)
{
  CDBG("hg_enter_powersave\n");
}

void hg_leave_powersave(void)
{
  CDBG("hg_leave_powersave\n");
}

static void hg_fix(void)
{
  CDBG("hg_fix hg_state = %bx\n", hg_state);
  
  serial_set_ctl_bit(SERIAL_BIT_HG0_FIX, (hg_state & 1) == 0);
  serial_set_ctl_bit(SERIAL_BIT_HG1_FIX, (hg_state & 2) == 0);  
  serial_set_ctl_bit(SERIAL_BIT_HG2_FIX, (hg_state & 4) == 0);
  serial_set_ctl_bit(SERIAL_BIT_HG3_FIX, (hg_state & 8) == 0);  
  serial_ctl_out();
}

void scan_hg(unsigned int status)
{
  unsigned char old_hg_state;
  CDBG("scan_hg %x\n", status);
  
  if(!hg_enabled) return;
  
  old_hg_state = hg_state;
  hg_state = (status & 0x0F00) >> 8;
  hg_state = ~hg_state;  
  hg_state &= 0xF;
  
  CDBG("scan_hg hg_state = %bx, old_state = %bx\n", hg_state, old_hg_state);
    
  if(old_hg_state != hg_state) {
    CDBG("EV_ROTATE_HG!\n");
    set_task(EV_ROTATE_HG);
    if(power_test_flag()) {
      power_clr_flag();
    }
    hg_fix();
  }
}


void hg_enable(bit enable)
{
  CDBG("hg_enable %bd\n", enable ? 1 : 0);
  if(enable && !hg_enabled) {
    hg_power_on();
  } else if(!enable && hg_enabled){
    hg_power_off();
  }
  
  hg_enabled = enable;
}

unsigned char hg_get_state(void)
{
  CDBG("hg_get_state\n");
  return hg_state;
}

