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
  IE1 = 0; // ����жϱ�־λ
  set_task(EV_SCAN_INT_HUB);
}

void int_hub_initialize (void)
{
  CDBG("int_hub_initialize\n");
  IT1 = 1; // ����Ϊ���ش���
  EX1 = 1; // ���ж�
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