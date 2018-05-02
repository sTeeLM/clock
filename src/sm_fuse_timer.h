#ifndef __CLOCK_SM_FUSE_TIMER_H__
#define __CLOCK_SM_FUSE_TIMER_H__

#include "sm.h"

enum sm_fuse_timer_ss {
  SM_FUSE_TIMER_INIT      = 0, // 初始状态
  SM_FUSE_TIMER_PREARMED  = 1, // 开启传感器阶段，有问题可以回滚
  SM_FUSE_TIMER_ARMED     = 2, // 真的armed了！
  SM_FUSE_TIMER_VERIFY    = 3, // 检查password
  SM_FUSE_TIMER_DISARMED  = 4, // 被解除了
  SM_FUSE_TIMER_PREDETONATE = 5, // 准备detonate
};

extern const char * code sm_fuse_timer_ss_name[];

void sm_fuse_timer_init(unsigned char from, unsigned char to, enum task_events);
void sm_fuse_timer_submod0(unsigned char from, unsigned char to, enum task_events);
void sm_fuse_timer_submod1(unsigned char from, unsigned char to, enum task_events);
void sm_fuse_timer_submod2(unsigned char from, unsigned char to, enum task_events);
void sm_fuse_timer_submod3(unsigned char from, unsigned char to, enum task_events);
void sm_fuse_timer_submod4(unsigned char from, unsigned char to, enum task_events);

#endif