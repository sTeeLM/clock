#ifndef __CLOCK_SM_FUSE_POWERSAVE_H__
#define __CLOCK_SM_FUSE_POWERSAVE_H__

#include "sm.h"

enum sm_fuse_powersave_ss {
  SM_FUSE_POWERSAVE_INIT      = 0, // ��ʼ״̬
  SM_FUSE_POWERSAVE_PS        = 1
};

void sm_fuse_powersave(unsigned char from, unsigned char to, enum task_events);

#endif