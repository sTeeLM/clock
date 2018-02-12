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
  SM_FUSE_PARAM_HG_ONOFF  = 7,
  SM_FUSE_PARAM_GYRO_ONOFF      = 8,
  SM_FUSE_PARAM_THERMO_HI_ONOFF = 9,
  SM_FUSE_PARAM_THERMO_LO_ONOFF = 10,
  SM_FUSE_PARAM_TRIPWIRE_ONOFF  = 11,
  SM_FUSE_PARAM_PASSWORD        = 12,
  
};

extern const char * code sm_fuse_param_ss_name[];

void sm_fuse_param(unsigned char from, unsigned char to, enum task_events);

#endif