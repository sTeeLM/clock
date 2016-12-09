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
  EV_ALARM            = 11, // 闹钟应该响起
  EV_COUNTER          = 12, // 计时器到时间
  EV_POWER_SAVE       = 13, // 应该进入PS状态 
  EV_250MS            = 14, // 大约每250ms转一下
  EV_1S               = 15, // 大约每1s转一下
  EV_COUNT  
};

enum sm_states
{
  SM_DISPLAY            = 0, // 一般显示状态
  SM_MODIFY_TIME        = 1, // 修改时间
  SM_MODIFY_ALARM       = 2, // 修改闹钟
  SM_MODIFY_GLOBAL_FLAG = 3, // 修改全局状态
  SM_PAC_HIT            = 4, // 节电/闹表/倒计时到时间
  SM_TIMER              = 5, // 秒表功能
  SM_COUNTER            = 6  // 倒计时功能
};

extern unsigned int ev_bits;

typedef void (*TASK_PROC)(enum task_events);
typedef void (*SM_PROC)(unsigned char from, unsigned char to, enum task_events ev);

struct sm_trans
{
  unsigned char from_state;
  unsigned char event;
  unsigned char to_state;
  SM_PROC sm_proc;
};

void task_initialize (void);

#define set_task(ev) \
  do{ev_bits |= 1<<ev;}while(0)

#define clr_task(ev) \
  do{ev_bits &= ~(1<<ev);}while(0)
    
#define  test_task(ev) \
  (ev_bits & (1<<ev))

#define get_sm_state(state) \
  (((state & 0xF0) >> 4) & 0x0F)

#define get_sm_ss_state(state) \
  (state & 0x0F)

void run_task(void);

void state_machine_proc(enum task_events);
  
void run_state_machine(enum task_events);

#endif