#ifndef __CLOCK_SM_TIMER_H__
#define __CLOCK_SM_TIMER_H__

#include "sm.h"

enum sm_timer_ss {
  SM_TIMER_INIT     = 0, //��ʼ״̬
  SM_TIMER_CLEAR    = 1, //����״̬
  SM_TIMER_RUNNING  = 2, //��ʱ״̬
  SM_TIMER_STOP     = 3, //ֹͣ״̬
};

void sm_timer(unsigned char from, unsigned char to, enum task_events);

#endif