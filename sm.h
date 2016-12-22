#ifndef __CLOCK_SM_H__
#define __CLOCK_SM_H__

#include "task.h"

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

typedef void (code *SM_PROC)(unsigned char from, unsigned char to, enum task_events ev);

struct sm_trans
{
  unsigned char from_state;
  enum task_events event;
  unsigned char to_state;
  SM_PROC sm_proc;
};


#define get_sm_state(st) \
  (((st & 0xF0) >> 4) & 0x0F)

#define get_sm_ss_state(st) \
  (st & 0x0F)

void run_state_machine(enum task_events);
void state_machine_timer_proc(enum task_events ev);
void sm_initialize (void);
#endif