#include <STC89C5xRC.H>

#include "int_hub.h"
#include "serial_hub.h"
#include "debug.h"
#include "power.h"
#include "rtc.h"
#include "beeper.h"
#include "task.h"
#include "hg.h"
#include "gyro.h"
#include "thermo.h"
#include "tripwire.h"
#include "fuse.h"

static void int1_ISR (void) interrupt 2 using 1
{
  IE1 = 0; // 清除中断标志位
  set_task(EV_SCAN_INT_HUB);
}

void int_hub_initialize (void)
{
  CDBG("int_hub_initialize\n");
  IT1 = 1; // 设置为边沿触发
  EX1 = 1; // 开中断
}

void scan_int_hub_proc (enum task_events ev)
{
  CDBG("scan_int_hub_proc\n");
  serial_state_in();
  scan_rtc();
  scan_fuse();
  scan_hg();
  scan_thermo(); 
  scan_gyro(); 
  scan_tripwire();  
}