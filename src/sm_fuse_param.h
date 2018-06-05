#ifndef __CLOCK_SM_FUSE_PARAM_H__
#define __CLOCK_SM_FUSE_PARAM_H__

#include "sm.h"

enum sm_fuse_param_ss {
  SM_FUSE_PARAM_INIT      = 0, // 初始状态
  SM_FUSE_PARAM_YY        = 1,
  SM_FUSE_PARAM_MO        = 2,
  SM_FUSE_PARAM_DD        = 3,
  SM_FUSE_PARAM_HH        = 4,
  SM_FUSE_PARAM_MM        = 5,
  SM_FUSE_PARAM_SS        = 6,
  SM_FUSE_PARAM_PASSWORD  = 7,
  
};

extern const char * code sm_fuse_param_ss_name[];

void sm_fuse_param_init(unsigned char from, unsigned char to, enum task_events);
void sm_fuse_param_submod0(unsigned char from, unsigned char to, enum task_events);
void sm_fuse_param_submod1(unsigned char from, unsigned char to, enum task_events);
void sm_fuse_param_submod2(unsigned char from, unsigned char to, enum task_events);
void sm_fuse_param_submod3(unsigned char from, unsigned char to, enum task_events);
void sm_fuse_param_submod4(unsigned char from, unsigned char to, enum task_events);
void sm_fuse_param_submod5(unsigned char from, unsigned char to, enum task_events);
void sm_fuse_param_submod6(unsigned char from, unsigned char to, enum task_events);

#endif
