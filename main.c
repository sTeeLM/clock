#include <REGX52.H>
#include <stdio.h>

#include "task.h"
#include "com.h"
#include "timer.h"
#include "rtc.h"
#include "key.h"
#include "led.h"
#include "alarm.h"
#include "power.h"
#include "count_down.h"
#include "debug.h"

void main(void)
 {	 
	EA = 1;                                       // enable global interrupts
  com_initialize();                             // initialize interrupt driven serial I/O
	task_initialize();
  timer_initialize(12e6);												// initialize timer
	rtc_initialize();
	key_initialize();
	led_initialize();
  power_initialize();
  alarm_initialize();
  count_down_initialize();
  run_task();                                   // run task procs
 }