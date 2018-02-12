#ifndef __CLOCK_SM_FUSE_MODE_H__
#define __CLOCK_SM_FUSE_MODE_H__

#include "sm.h"

enum sm_fuse_mode_ss {
  SM_FUSE_MODE_INIT      = 0, // 初始状态
  SM_FUSE_MODE_TIMER     = 1, // 定时防拆模式
  SM_FUSE_MODE_GRENADE   = 2, // 碰撞触发测试
};

extern const char * code sm_fuse_mode_ss_name[];

void sm_fuse_mode(unsigned char from, unsigned char to, enum task_events);

#endif