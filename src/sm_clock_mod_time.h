#ifndef __CLOCK_SM_CLOCK_MOD_TIME_H__
#define __CLOCK_SM_CLOCK_MOD_TIME_H__

#include "sm.h"

enum sm_clock_mod_time_ss {
  SM_CLOCK_MODIFY_TIME_INIT = 0, // 初始状态
  SM_CLOCK_MODIFY_TIME_HH   = 1, // 修改时
  SM_CLOCK_MODIFY_TIME_MM   = 2, // 修改分
  SM_CLOCK_MODIFY_TIME_SS   = 3, // 修改分  
  SM_CLOCK_MODIFY_TIME_YY   = 4, // 修改年
  SM_CLOCK_MODIFY_TIME_MO   = 5, // 修改月
  SM_CLOCK_MODIFY_TIME_DD   = 6, // 修改日   
};

extern const char * code sm_clock_mod_time_name[];

void sm_clock_mod_time_init(unsigned char from, unsigned char to, enum task_events);
void sm_clock_mod_time_submod0(unsigned char from, unsigned char to, enum task_events);
void sm_clock_mod_time_submod1(unsigned char from, unsigned char to, enum task_events);
void sm_clock_mod_time_submod2(unsigned char from, unsigned char to, enum task_events);
void sm_clock_mod_time_submod3(unsigned char from, unsigned char to, enum task_events);
void sm_clock_mod_time_submod4(unsigned char from, unsigned char to, enum task_events);
void sm_clock_mod_time_submod5(unsigned char from, unsigned char to, enum task_events);
#endif