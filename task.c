#include <string.h>

#include "task.h"
#include "sm.h"

/* hardware*/
#include "timer.h"
#include "rtc.h"
#include "key.h"
#include "led.h"
#include "alarm.h"
#include "power.h"
#include "counter.h"
#include "debug.h"

/*
enum task_events
{
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
  EV_ALARM            = 13, // 闹钟应该响起
  EV_COUNTER          = 14, // 计时器到时间
  EV_POWER_SAVE       = 15, // 应该进入PS状态 
  EV_COUNT  
};
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
  alarm_proc,
  counter_proc,
  power_proc  
};


unsigned int idata ev_bits;

void task_initialize (void)
{
  CDBG("task_initialize\n");
  ev_bits = 0;
}

void run_task(void)
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

