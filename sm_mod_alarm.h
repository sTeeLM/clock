#ifndef __CLOCK_SM_MOD_ALARM_H__
#define __CLOCK_SM_MOD_ALARM_H__

#include "sm.h"

enum sm_mod_alarm_ss {
  SM_MODIFY_ALARM_INIT= 0, // 初始状态
  SM_MODIFY_ALARM_HH = 1, // 修改Alarm时
  SM_MODIFY_ALARM_MM = 2, // 修改Alarm分
  SM_MODIFY_ALARM_ON = 3, // 修改Alarm on/off
};

void sm_mod_alarm(unsigned char from, unsigned char to, enum task_events);

#endif