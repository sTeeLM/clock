#ifndef __CLOCK_SM_MOD_ALARM_H__
#define __CLOCK_SM_MOD_ALARM_H__

#include "task.h"

enum sm_mod_alarm_ss {
  SM_MODIFY_ALARM_INIT= 0, // 初始状态
  SM_MODIFY_ALARM0_HH = 1, // 修改Alarm 0时
  SM_MODIFY_ALARM0_MM = 2, // 修改Alarm 0分
  SM_MODIFY_ALARM0_ON = 3, // 修改Alarm 0分  
  SM_MODIFY_ALARM1_HH = 4, // 修改Alarm 1时
  SM_MODIFY_ALARM1_MM = 5, // 修改Alarm 1分
  SM_MODIFY_ALARM1_ON = 6, // 修改Alarm 1分  
};

void sm_mod_alarm(unsigned char from, unsigned char to, enum task_events);

#endif