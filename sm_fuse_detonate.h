#ifndef __CLOCK_SM_FUSE_DETONATE_H__
#define __CLOCK_SM_FUSE_DETONATE_H__

#include "sm.h"

enum sm_fuse_detonate_ss {
  SM_FUSE_DETONATE_INIT      = 0, // ��ʼ״̬
  SM_FUSE_DETONATE_CHARGE    = 1, // boom
};

void sm_fuse_detonate(unsigned char from, unsigned char to, enum task_events);

#endif