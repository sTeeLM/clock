#include <string.h>

#include "task.h"
#include "sm.h"

/* hardware*/
#include "clock.h"
#include "rtc.h"
#include "key.h"
#include "led.h"
#include "alarm.h"
#include "power.h"
#include "timer.h"
#include "fuse.h"
#include "int_hub.h"
#include "hg.h"
#include "thermo.h"
#include "gyro.h"
#include "tripwire.h"

#include "debug.h"


/*
  EV_250MS            = 0, // 大约每250ms转一下
  EV_1S               = 1, // 大约每1s转一下  
  EV_SCAN_KEY         = 2, // 扫描按键
  EV_KEY_MOD_DOWN     = 3, // mod键按下
  EV_KEY_MOD_UP       = 4, // mod键抬起
  EV_KEY_MOD_PRESS    = 5, // mod键短按
  EV_KEY_MOD_LPRESS   = 6, // mod键长按  
  EV_KEY_SET_DOWN     = 7, // set键按下
  EV_KEY_SET_UP       = 8, // set键抬起
  EV_KEY_SET_PRESS    = 9, // set键短按
  EV_KEY_SET_LPRESS   = 10, // set键长按
  EV_KEY_MOD_SET_PRESS    = 11, // mod set键同时短按
  EV_KEY_MOD_SET_LPRESS   = 12, // mod set 键同时长按 
  EV_SCAN_INT_HUB     = 13, // 扫描fuse，hg，gyro
  EV_FUSE0_SHORT_BROKE = 14, // fuse被剪断或者短路
  EV_FUSE1_SHORT_BROKE = 15, // fuse被剪断或者短路
  EV_HG            = 16, // hg检测出来倾斜状态改变
  EV_GYRO          = 17, // gyro检测出来倾斜状态改变
  EV_THERMO_LO     = 18, // 温度太低
  EV_THERMO_HI     = 19, // 温度太高
  EV_FUSE0_TRIGGER     = 20, // fuse应该被触发 
  EV_FUSE1_TRIGGER     = 21,
  EV_TRIPWIRE         = 22,
  EV_ALARM0           = 23, // 闹钟0应该响起
  EV_ALARM1           = 24, // 闹钟1应该响起
  EV_COUNTER          = 25, // 计时器到时间
  EV_POWER_SAVE       = 26, // 应该进入PS状态 
  EV_COUNT  
*/

static const TASK_PROC code task_procs[EV_COUNT] = 
{
  state_machine_timer_proc,
  state_machine_timer_proc,
  scan_key_proc,
  mod_proc,
  mod_proc,
  mod_proc,
  mod_proc,  
  set_proc,
  set_proc,
  set_proc,
  set_proc,
  mod_set_proc,
  mod_set_proc, 
  scan_int_hub_proc,
  fuse_proc,
  fuse_proc,
  hg_proc,
  gyro_proc,
  thermo_proc,
  thermo_proc,  
  fuse_proc,
  fuse_proc,
  tripwire_proc,
  alarm_proc,
  alarm_proc,
  timer_proc,
  power_proc  
};


unsigned int idata ev_bits0;
unsigned int idata ev_bits1;

void task_initialize (void)
{
  CDBG("task_initialize\n");
  ev_bits0 = 0;
  ev_bits1 = 0;
}

void run_task()
{
  unsigned char c;
  while(1) {
    for(c = 0; c < EV_COUNT; c++) {
      if(test_task(c)) {
        task_procs[c](c);
        clr_task(c);
      }
    }
  }
}


