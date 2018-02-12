#ifndef __CLOCK_SM_CLOCK_COUNTER_H__
#define __CLOCK_SM_CLOCK_COUNTER_H__

#include "sm.h"

enum sm_clock_counter_ss {
  SM_CLOCK_COUNTER_INIT       = 0, // 初始状态
  SM_CLOCK_COUNTER_MODIFY_HH  = 1, // 设置时
  SM_CLOCK_COUNTER_MODIFY_MM  = 2, // 设置分
  SM_CLOCK_COUNTER_MODIFY_SS  = 3, // 设置秒
  SM_CLOCK_COUNTER_RUNNING    = 4, // 倒计时状态
  SM_CLOCK_COUNTER_PAUSE      = 5, // 暂停状态
  SM_CLOCK_COUNTER_STOP       = 6, // 停止/响铃状态
};

extern const char * code sm_clock_counter_ss_name[];
  
void sm_clock_counter(unsigned char from, unsigned char to, enum task_events);

#endif