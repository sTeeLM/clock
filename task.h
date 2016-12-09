#ifndef __CLOCK_TASK_H__
#define __CLOCK_TASK_H__

enum task_events
{
  EV_SCAN_KEY         = 0, // 扫描按键
  EV_KEY_MOD_DOWN     = 1, // mod键按下
  EV_KEY_MOD_UP       = 2, // mod键抬起
  EV_KEY_MOD_PRESS    = 3, // mod键短按
  EV_KEY_MOD_LPRESS   = 4, // mod键长按  
  EV_KEY_SET_DOWN     = 5, // set键按下
  EV_KEY_SET_UP       = 6, // set键抬起
  EV_KEY_SET_PRESS    = 7, // set键短按
  EV_KEY_SET_LPRESS   = 8, // set键长按
  EV_KEY_MOD_SET_PRESS    = 9, // mod set键同时短按
  EV_KEY_MOD_SET_LPRESS   = 10, // mod set 键同时长按    
  EV_ALARM0           = 11, // 闹钟0应该响起
  EV_ALARM1           = 12, // 闹钟1应该响起
  EV_COUNT_DOWN       = 13, // 计时器到时间
  EV_POWER_SAVE       = 14, // 应该进入PS状态 
  EV_COUNT  
};

extern unsigned int ev_cnt;

typedef void (*TASK_PROC)(enum task_events);

void task_initialize (void);

#define set_task(ev) \
  do{ev_cnt |= 1<<ev;}while(0)

#define clr_task(ev) \
  do{ev_cnt &= ~(1<<ev);}while(0)
    
#define  test_task(ev) \
  (ev_cnt & (1<<ev))

void run_task(void);
#endif