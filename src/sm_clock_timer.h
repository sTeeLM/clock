#ifndef __CLOCK_SM_CLOCK_TIMER_H__
#define __CLOCK_SM_CLOCK_TIMER_H__

#include "sm.h"

enum sm_clock_timer_ss {
  SM_CLOCK_TIMER_INIT     = 0, //初始状态
  SM_CLOCK_TIMER_CLEAR    = 1, //清零状态
  SM_CLOCK_TIMER_RUNNING  = 2, //计时状态
  SM_CLOCK_TIMER_STOP     = 3, //停止状态
};

extern const char * code sm_clock_timer_ss_name[];

void sm_clock_timer_init(unsigned char from, unsigned char to, enum task_events);
void sm_clock_timer_submod0(unsigned char from, unsigned char to, enum task_events);
void sm_clock_timer_submod1(unsigned char from, unsigned char to, enum task_events);
void sm_clock_timer_submod2(unsigned char from, unsigned char to, enum task_events);
#endif
