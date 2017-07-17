#ifndef __CLOCK_SM_FUSE_TIMER_H__
#define __CLOCK_SM_FUSE_TIMER_H__

#include "sm.h"

enum sm_fuse_timer_ss {
  SM_FUSE_TIMER_INIT      = 0, // 初始状态
  SM_FUSE_TIMER_PREARMED  = 1, // 开始检查和设置参数
  SM_FUSE_TIMER_ARMED     = 2, // 真的armed了！
  SM_FUSE_TIMER_VERIFY    = 3, // 检查password
  SM_FUSE_TIMER_DISARMED  = 4, // 被解除了
  SM_FUSE_TIMER_PREDETONATE = 5, // 准备detonate
};

void sm_fuse_timer(unsigned char from, unsigned char to, enum task_events);

#endif