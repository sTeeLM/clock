#ifndef __CLOCK_SM_FUSE_POWERSAVE_H__
#define __CLOCK_SM_FUSE_POWERSAVE_H__

#include "sm.h"

enum sm_fuse_powersave_ss {
  SM_FUSE_POWERSAVE_INIT      = 0, // 初始状态
  SM_FUSE_POWERSAVE_PS        = 1, // 节电状态
  SM_FUSE_POWERSAVE_PREDETONATE = 2, // 触发准备状态
  SM_FUSE_POWERSAVE_DISARM    = 3, // 解除准备状态
};

extern const char * code sm_fuse_powersave_ss_name[];

void sm_fuse_powersave_init(unsigned char from, unsigned char to, enum task_events);
void sm_fuse_powersave_submod0(unsigned char from, unsigned char to, enum task_events);
void sm_fuse_powersave_submod1(unsigned char from, unsigned char to, enum task_events);
void sm_fuse_powersave_submod2(unsigned char from, unsigned char to, enum task_events);

#endif
