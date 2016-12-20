#ifndef __CLOCK_TASK_H__
#define __CLOCK_TASK_H__

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

extern unsigned int idata ev_bits;

typedef void (code *TASK_PROC)(enum task_events);

void task_initialize (void);

#define set_task(ev1) \
  do{ev_bits |= 1<<ev1;}while(0)

#define clr_task(ev1) \
  do{ev_bits &= ~(1<<ev1);}while(0)
    
#define  test_task(ev1) \
  (ev_bits & (1<<ev1))

void run_task(void);

#endif