#include <STC89C5xRC.H>
#include <stdio.h>

#include "task.h"
#include "sm.h"
#include "com.h"
#include "timer.h"
#include "rtc.h"
#include "key.h"
#include "led.h"
#include "alarm.h"
#include "power.h"
#include "counter.h"
#include "beeper.h"
#include "debug.h"
#include "cext.h"

void main(void)
 {	 
	EA = 1;                                       // enable global interrupts
  com_initialize();                             // initialize interrupt driven serial I/O
  timer_initialize(12e6);												// initialize timer
  rtc_initialize();                             // initialize rtc
  key_initialize();                             // initialize keys
  led_initialize();                             // initialize led display
  power_initialize();                           // initialize power manager
  alarm_initialize();                           // initialize power alarm manager
  counter_initialize();                         // initialize counter
  beeper_initialize();                          // initialize beeper
  task_initialize();                            // initialize task manager
  sm_initialize();                              // initialize state machine
  run_task();                                   // run task procs, loop forever
 }