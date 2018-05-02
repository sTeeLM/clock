#include <STC89C5xRC.H>
#include <stdio.h>

#include "task.h"
#include "sm.h"
#include "com.h"
#include "clock.h"
#include "rtc.h"
#include "key.h"
#include "led.h"
#include "alarm.h"
#include "power.h"
#include "timer.h"
#include "beeper.h"
#include "fuse.h"
#include "mpu.h"
#include "hg.h"
#include "thermo.h"
#include "int_hub.h"
#include "serial_hub.h"
#include "lt_timer.h"
#include "debug.h"
#include "cext.h"
#include "rom.h"
#include "shell.h"


#define __CLOCK__VERSION__ "2.0.0.0"


static void show_version(void)
{
  
  CDBG("++++++++++++++++++++++++++++++++++++++++\n");
  CDBG("+        tiny OS  %s              +\n", __CLOCK__VERSION__);
  CDBG("+                                      +\n");
  CDBG("++++++++++++++++++++++++++++++++++++++++\n");
  
}

void main(void)
{  
  EA = 1;                                       // enable global interrupts
  debug_initialize();                           // initialize debug system
  com_initialize();                             // initialize interrupt driven serial I/O
  show_version();
  rom_initialize();                             // initialize rom
  serial_hub_initialize();                      // initialize serial hub
  rtc_initialize();                             // initialize rtc
  clock_initialize();                           // initialize clock
  key_initialize();                             // initialize keys
  led_initialize();                             // initialize led display
  power_initialize();                           // initialize power manager
  alarm_initialize();                           // initialize power alarm manager
  lt_timer_initialize();                        // initialize lt_timer
  timer_initialize();                           // initialize timer
  beeper_initialize();                          // initialize beeper
  thermo_initialize();                          // initialize thermo
  fuse_initialize();                            // initialize fuse
  mpu_initialize();                            // initialize mpu
  hg_initialize();                              // initialize hg
  int_hub_initialize();                         // initialize interrupt hub 
  task_initialize();                            // initialize task manager
  sm_initialize();                              // initialize state machine

  while(1) {                                    //loop forever
    run_task();                                 // run task procs
    run_shell();                                // run shell
  }
}