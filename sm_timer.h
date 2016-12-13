#ifndef __CLOCK_SM_TIMER_H__
#define __CLOCK_SM_TIMER_H__

#include "sm.h"

enum sm_timer_ss {
  SM_TIMER_INIT     = 0, //³õÊ¼×´Ì¬
  SM_TIMER_CLEAR    = 1, //ÇåÁã×´Ì¬
  SM_TIMER_RUNNING  = 2, //¼ÆÊ±×´Ì¬
  SM_TIMER_STOP     = 3, //Í£Ö¹×´Ì¬
};

void sm_timer(unsigned char from, unsigned char to, enum task_events);

#endif