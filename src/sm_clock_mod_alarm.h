#ifndef __CLOCK_SM_CLOCK_MOD_ALARM_H__
#define __CLOCK_SM_CLOCK_MOD_ALARM_H__

#include "sm.h"

enum sm_clock_mod_alarm_ss {
  SM_CLOCK_MODIFY_ALARM_INIT = 0, // 初始状态
  SM_CLOCK_MODIFY_ALARM_HH   = 1, // 修改Alarm时
  SM_CLOCK_MODIFY_ALARM_MM   = 2, // 修改Alarm分
  SM_CLOCK_MODIFY_ALARM_DAY1 = 3,  // 修改Alarm星期一开启状态
  SM_CLOCK_MODIFY_ALARM_DAY2 = 4,  // 修改Alarm星期二开启状态
  SM_CLOCK_MODIFY_ALARM_DAY3 = 5,  // 修改Alarm星期三开启状态
  SM_CLOCK_MODIFY_ALARM_DAY4 = 6,  // 修改Alarm星期四开启状态
  SM_CLOCK_MODIFY_ALARM_DAY5 = 7,  // 修改Alarm星期五开启状态
  SM_CLOCK_MODIFY_ALARM_DAY6 = 8,  // 修改Alarm星期六开启状态
  SM_CLOCK_MODIFY_ALARM_DAY7 = 9  // 修改Alarm星期日开启状态  
};

extern const char * code sm_clock_mod_alarm_ss_name[];

void sm_clock_mod_alarm(unsigned char from, unsigned char to, enum task_events);

#endif