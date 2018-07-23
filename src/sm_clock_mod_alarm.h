#ifndef __CLOCK_SM_CLOCK_MOD_ALARM_H__
#define __CLOCK_SM_CLOCK_MOD_ALARM_H__

#include "sm.h"

enum sm_clock_mod_alarm_ss {
  SM_CLOCK_MODIFY_ALARM_INIT = 0, // 初始状态
  SM_CLOCK_MODIFY_ALARM_HH   = 1, // 修改Alarm时
  SM_CLOCK_MODIFY_ALARM_MM   = 2, // 修改Alarm分
  SM_CLOCK_MODIFY_ALARM_DAY  = 3,  // 修改Alarm星期几ON/OFF状态
  SM_CLOCK_MODIFY_ALARM_BS   = 4,  // 整点报时ON/OFF
  SM_CLOCK_MODIFY_ALARM_DUR  = 5,  // 闹铃持续时间
};

extern const char * code sm_clock_mod_alarm_ss_name[];

void sm_clock_mod_alarm_init(unsigned char from, unsigned char to, enum task_events);
void sm_clock_mod_alarm_submod0(unsigned char from, unsigned char to, enum task_events);
void sm_clock_mod_alarm_submod1(unsigned char from, unsigned char to, enum task_events);
void sm_clock_mod_alarm_submod2(unsigned char from, unsigned char to, enum task_events);
void sm_clock_mod_alarm_submod3(unsigned char from, unsigned char to, enum task_events);
void sm_clock_mod_alarm_submod4(unsigned char from, unsigned char to, enum task_events);
#endif
