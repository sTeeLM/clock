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
#include "delay_task.h"
#include "remote.h"
#include "indicator.h"
#include "radio.h"
#include "misc.h"


#define __CLOCK__VERSION__ "2.0.0.0"


static void show_version(void)
{
  
  CDBG("++++++++++++++++++++++++++++++++++++++++\n");
  CDBG("+        tiny OS  %s              +\n", __CLOCK__VERSION__);
  CDBG("+        %s                       +\n", 
#ifdef __CLOCK_EMULATE__
  "emulate"
#else
  "       "
#endif
  );
  CDBG("++++++++++++++++++++++++++++++++++++++++\n");
  
}

void main(void)
{  
  delay_ms(100);
  EA = 1;                                       // enable global interrupts
  debug_initialize();                           // initialize debug system
  com_initialize();                             // initialize interrupt driven serial I/O
  show_version();
  rom_initialize();                             // initialize rom
  task_initialize();                            // initialize task manager
  sm_initialize();                              // initialize state machine
  delay_task_initialize();                      // initialize delay task
  serial_hub_initialize();                      // initialize serial hub
  int_hub_initialize();                         // initialize interrupt hub 
  
  led_initialize();                             // initialize led display
  power_initialize();                           // initialize power manager
  radio_initialize();                           // initialize radio
  indicator_initialize();                       // initialize indicator
  key_initialize();                             // initialize keys
  timer_initialize();                           // initialize timer
  beeper_initialize();                          // initialize beeper
  thermo_initialize();                          // initialize thermo
  fuse_initialize();                            // initialize fuse
  hg_initialize();                              // initialize hg
  remote_initialize();                          // initialize remote control
  mpu_initialize();                             // initialize mpu
  rtc_initialize();                             // initialize rtc
  
  clock_initialize();                           // initialize clock
  alarm_initialize();                           // initialize power alarm manager
  lt_timer_initialize();                        // initialize lt_timer

  while(1) {                                    // loop forever
    run_task();                                 // run task procs
    run_shell();                                // run shell
  }
}
