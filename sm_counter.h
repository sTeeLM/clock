#ifndef __CLOCK_SM_COUNTER_H__
#define __CLOCK_SM_COUNTER_H__

#include "task.h"

enum sm_counter_ss {
  SM_COUNTER_INIT       = 0, // 初始状态
  SM_COUNTER_MODIFY_HH  = 1, // 设置时
  SM_COUNTER_MODIFY_MM  = 2, // 设置分
  SM_COUNTER_MODIFY_SS  = 3, // 设置秒
  SM_COUNTER_RUNNING    = 4, // 倒计时状态
  SM_COUNTER_STOP       = 5, // 停止/暂停状态
};

void sm_counter(unsigned char from, unsigned char to, enum task_events);

#endif