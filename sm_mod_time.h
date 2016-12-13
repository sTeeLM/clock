#ifndef __CLOCK_SM_MOD_TIME_H__
#define __CLOCK_SM_MOD_TIME_H__

#include "sm.h"

enum sm_mod_time_ss {
  SM_MODIFY_TIME_INIT = 0, // 初始状态
  SM_MODIFY_TIME_HH   = 1, // 修改时
  SM_MODIFY_TIME_MM   = 2, // 修改分
  SM_MODIFY_TIME_SS   = 3, // 修改分  
  SM_MODIFY_TIME_YY   = 4, // 修改年
  SM_MODIFY_TIME_MO   = 5, // 修改月
  SM_MODIFY_TIME_DD   = 6, // 修改日   
};

void sm_mod_time(unsigned char from, unsigned char to, enum task_events);

#endif