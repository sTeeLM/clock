#include <STC89C5xRC.H>

#include "power.h"
#include "task.h"
#include "sm.h"
#include "timer.h"
#include "com.h"
#include "timer.h"
#include "rtc.h"
#include "key.h"
#include "led.h"
#include "alarm.h"
#include "counter.h"
#include "beeper.h"
#include "debug.h"
#include "cext.h"

static unsigned char powersave_to_s;
static unsigned char last_ps_s;

void power_initialize(void)
{
  CDBG("power_initialize\n");
  powersave_to_s = 15;
}

void power_proc(enum task_events ev)
{
  CDBG("power_proc %bd\n", ev);
  run_state_machine(ev);
}

void power_enter_powersave(void)
{
  CDBG("power_enter_powersave\n");
  timer_enter_powersave();
  rtc_enter_powersave();       
  key_enter_powersave();       
  led_enter_powersave();          
  alarm_enter_powersave();     
  counter_enter_powersave();   
  beeper_enter_powersave();
  com_enter_powersave();
  PCON |= 0x1;
}

void power_leave_powersave(void)
{
  com_leave_powersave(); 
  beeper_leave_powersave(); 
  counter_leave_powersave(); 
  alarm_leave_powersave();  
  led_leave_powersave(); 
  key_leave_powersave(); 
  rtc_leave_powersave(); 
  timer_leave_powersave();
  CDBG("power_leave_powersave\n");
}

unsigned char get_powersave_to_s(void)
{
  return powersave_to_s;
}

enum powersave_time get_powersave_to(void)
{
  switch(powersave_to_s) {
    case 0:  return POWERSAVE_OFF;
    case 15: return POWERSAVE_15S;
    case 30: return POWERSAVE_30S;
  }
  return POWERSAVE_OFF;
}

void set_powersave_to(enum powersave_time to)
{
  switch(to) {
    case POWERSAVE_OFF:
      powersave_to_s = 0;  break;
    case POWERSAVE_15S: 
      powersave_to_s = 15; break;
    case POWERSAVE_30S: 
      powersave_to_s = 30; break;
  }
}

bit test_powersave_to(void)
{
  if(powersave_to_s != 0 
    && time_diff(counter_1s, last_ps_s) >= powersave_to_s) {
      CDBG("test_powersave_to time out!\n");
      set_task(EV_POWER_SAVE);
      return 1;
  }
  return 0;
}


void reset_powersave_to(void)
{
  last_ps_s = counter_1s;
}