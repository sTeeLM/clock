#ifndef __CLOCK_SM_FUSE_DETONATE_H__
#define __CLOCK_SM_FUSE_DETONATE_H__

#include "sm.h"

enum sm_fuse_detonate_ss {
  SM_FUSE_DETONATE_INIT      = 0, // 初始状态
  SM_FUSE_DETONATE_CHARGE    = 1, // boom
};

extern const char * code sm_fuse_detonate_ss_name[];

void sm_fuse_detonate_init(unsigned char from, unsigned char to, enum task_events ev);
void sm_fuse_detonate_submod0(unsigned char from, unsigned char to, enum task_events ev);

#endif