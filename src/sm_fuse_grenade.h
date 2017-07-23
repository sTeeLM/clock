#ifndef __CLOCK_SM_FUSE_GRENADE_H__
#define __CLOCK_SM_FUSE_GRENADE_H__

#include "sm.h"

enum sm_fuse_grenade_ss {
  SM_FUSE_GRENADE_INIT        = 0, // 初始状态
  SM_FUSE_GRENADE_PREARMED    = 1, // 预待触发模式
  SM_FUSE_GRENADE_ARMED       = 2, // 待触发模式
  SM_FUSE_GRENADE_DISARMED    = 3, // 解除状态
  SM_FUSE_GRENADE_PREDETONATE = 4  // 预DETONATE
};

void sm_fuse_grenade(unsigned char from, unsigned char to, enum task_events);

#endif