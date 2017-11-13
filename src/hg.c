#include "hg.h"
#include "debug.h"
#include "task.h"
#include "serial_hub.h"
#include "sm.h"
#include "power.h"
#include "delay_task.h"

static bit hg_enabled;
static unsigned char hg_state; // 只有低四位有效

static void hg_set_enable(void)
{
  hg_enabled = 1;
}

static void hg_off_flash(void)
{
  hg_enabled = 1;
}

static void hg_power_on(void)
{
  CDBG("hg_power_on\n");
  
  serial_set_ctl_bit(SERIAL_BIT_HG_EN, 0);  
  
  serial_ctl_out();
    
  hg_state = 0xF;

  delay_task_reg(hg_set_enable, 1);
  
  // hg_enabled = 1; set by hg_delay_task
}

static void hg_power_off(void)
{
  CDBG("hg_power_off\n");
  serial_set_ctl_bit(SERIAL_BIT_HG_EN, 1);
  serial_ctl_out();
  
  hg_enabled = 0;
}

void hg_initialize (void)
{
  CDBG("hg_initialize\n");
  hg_enabled = 0;
  hg_state   = 0xF;
}

void hg_enter_powersave(void)
{
  CDBG("hg_enter_powersave\n");
}

void hg_leave_powersave(void)
{
  CDBG("hg_leave_powersave\n");
}

void scan_hg(unsigned int status)
{
  unsigned char hg_new_state;
  CDBG("scan_hg %x\n", status);
  
  hg_new_state = (status & 0x0F00) >> 8; 
  hg_new_state &= 0xF;
  
  CDBG("scan_hg hg_state = %bx, hg_new_state = %bx\n", hg_state, hg_new_state);
  
  if(hg_new_state != hg_state) {
    if(hg_enabled) {
      CDBG("EV_ROTATE_HG!\n");
      set_task(EV_ROTATE_HG);
      delay_task_reg(hg_off_flash, 1);
    }
    if(power_test_flag()) {
      power_clr_flag();
    }
    hg_state = hg_new_state;
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
}

unsigned char hg_get_state(void)
{
  CDBG("hg_get_state return %bx\n", hg_state);
  return hg_state;
}

