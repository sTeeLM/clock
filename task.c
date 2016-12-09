#include <string.h>

#include "task.h"

#include "timer.h"
#include "rtc.h"
#include "key.h"
#include "led.h"
#include "alarm.h"
#include "power.h"
#include "count_down.h"
#include "debug.h"

/*
enum task_events
{
  EV_DISPLAY          = 0, // 刷数码管
  EV_SCAN_KEY         = 1, // 检查按键抬起
  EV_KEY_MOD_DOWN     = 2, // mod键按下
  EV_KEY_MOD_UP       = 3, // mod键抬起
  EV_KEY_MOD_PRESS    = 4, // mod键短按
  EV_KEY_MOD_LPRESS   = 5, // mod键长按  
  EV_KEY_SET_DOWN     = 6, // set键按下
  EV_KEY_SET_UP       = 7, // set键抬起
  EV_KEY_SET_PRESS    = 8, // set键短按
  EV_KEY_SET_LPRESS   = 9, // set键长按
  EV_KEY_MOD_SET_PRESS    = 10, // mod set键同时短按
  EV_KEY_MOD_SET_LPRESS   = 11, // mod键同时长按    
  EV_ALARM0           = 12, // 闹钟0应该响起
  EV_ALARM1           = 13, // 闹钟1应该响起
  EV_COUNT_DOWN       = 14, // 计时器到时间
  EV_POWER_SAVE       = 15, // 应该进入PS状态 
  EV_COUNT  
}; */

static const TASK_PROC code task_procs[EV_COUNT] = 
{
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
  alarm_proc,
  count_down_proc,
  power_proc,
};

unsigned int ev_cnt;

void task_initialize (void)
{
  CDBG("task_initialize\n");
  ev_cnt = 0;
}

void run_task(void)
{
  unsigned char c;
  while(1) {
    for(c = 0; c < EV_COUNT; c++) {
      if(test_task(c))
        task_procs[c](c);
    }
  }
}