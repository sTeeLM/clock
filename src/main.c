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
#include "gyro.h"
#include "hg.h"
#include "thermo.h"
#include "tripwire.h"
#include "int_hub.h"
#include "serial_hub.h"
#include "lt_timer.h"
#include "debug.h"
#include "cext.h"
#include "rom.h"


#define __CLOCK__VERSION__ "1.0.0.0"


static void show_version(void)
{
  
  CDBG("++++++++++++++++++++++++++++++++++++++++\n");
  CDBG("+        CLOCK & FUSE %s          +\n", __CLOCK__VERSION__);
  CDBG("+                                      +\n");
  CDBG("+        sTeeL<steel.mental@gmail.com> +\n");
  CDBG("++++++++++++++++++++++++++++++++++++++++\n");
  
}

void main(void)
{  
  EA = 1;                                       // enable global interrupts
  com_initialize();                             // initialize interrupt driven serial I/O
  show_version();
  rom_initialize();                             // initialize rom
  int_hub_initialize();                         // initialize interrupt hub 
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
  gyro_initialize();                            // initialize gyro
  hg_initialize();                              // initialize hg
  tripwire_initialize();                        // initialize tripwire
  task_initialize();                            // initialize task manager
  sm_initialize();                              // initialize state machine

  run_task();                                   // run task procs, loop forever
}