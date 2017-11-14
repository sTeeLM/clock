#ifndef __CLOCK_SM_CLOCK_ALARM_H__
#define __CLOCK_SM_CLOCK_ALARM_H__

#include "sm.h"

enum sm_clock_alarm_ss {
  SM_CLOCK_ALARM_HIT_ALARM0     = 0, //一般闹钟到时间
  SM_CLOCK_ALARM_HIT_ALARM1     = 1, // 整点报时到时间
};

extern const char * code sm_clock_alarm_ss_name[];

void sm_clock_alarm(unsigned char from, unsigned char to, enum task_events);
#endif