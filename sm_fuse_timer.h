#ifndef __CLOCK_SM_FUSE_TIMER_H__
#define __CLOCK_SM_FUSE_TIMER_H__

#include "sm.h"

enum sm_fuse_timer_ss {
  SM_FUSE_TIMER_INIT      = 0, // ³õÊ¼×´Ì¬
  SM_FUSE_TIMER_ARMED     = 1,
  SM_FUSE_TIMER_VERIFY    = 2,
  
};

void sm_fuse_timer(unsigned char from, unsigned char to, enum task_events);

#endif