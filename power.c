#include <STC89C5xRC.H>

#include "power.h"
#include "task.h"
#include "sm.h"
#include "clock.h"
#include "com.h"
#include "rtc.h"
#include "key.h"
#include "led.h"
#include "alarm.h"
#include "timer.h"
#include "beeper.h"
#include "int_hub.h"
#include "debug.h"
#include "cext.h"
#include "misc.h"

static unsigned char powersave_to_s;
static unsigned char last_ps_s;
bit powersave_flag;

void power_initialize(void)
{
  CDBG("power_initialize\n");
  powersave_to_s = 0;
  powersave_flag = 0;
}

void power_proc(enum task_events ev)
{
  CDBG("power_proc %bd\n", ev);
  run_state_machine(ev);
}

void power_enter_powersave(void)
{
  CDBG("power_enter_powersave\n");
  powersave_flag = 1;
  clock_enter_powersave();
  rtc_enter_powersave();       
  key_enter_powersave();       
  led_enter_powersave();          
  alarm_enter_powersave();     
  timer_enter_powersave();   
  beeper_enter_powersave();
  com_enter_powersave();
  while(powersave_flag) {
    PCON |= 0x1;
    scan_int_hub_proc(EV_SCAN_INT_HUB);
    if(!powersave_flag) {
      break;
    }
  }
}

void power_leave_powersave(void)
{
  com_leave_powersave(); 
  beeper_leave_powersave(); 
  timer_leave_powersave(); 
  alarm_leave_powersave();  
  led_leave_powersave(); 
  key_leave_powersave(); 
  rtc_leave_powersave(); 
  clock_leave_powersave();
  CDBG("power_leave_powersave\n");
}

unsigned char power_get_powersave_to_s(void)
{
  return powersave_to_s;
}

enum powersave_time power_get_powersave_to(void)
{
  switch(powersave_to_s) {
    case 0:  return POWERSAVE_OFF;
    case 15: return POWERSAVE_15S;
    case 30: return POWERSAVE_30S;
  }
  return POWERSAVE_OFF;
}

void power_inc_powersave_to(void)
{
  switch(powersave_to_s) {
    case 0:
      powersave_to_s = 15;  break;
    case 15: 
      powersave_to_s = 30; break;
    case 30: 
      powersave_to_s = 0; break;
  }
}

bit power_test_powersave_to(void)
{
  if(powersave_to_s != 0 
    && time_diff(clock_get_sec(), last_ps_s) >= powersave_to_s) {
      CDBG("test_powersave_to time out!\n");
      set_task(EV_POWER_SAVE);
      return 1;
  }
  return 0;
}

bit power_test_flag(void)
{
  return powersave_flag;
}

void power_set_flag(void)
{
  powersave_flag = 1;
}

void power_clr_flag(void)
{
  powersave_flag = 0;
}

void power_reset_powersave_to(void)
{
  last_ps_s = clock_get_sec();
}