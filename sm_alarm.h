#ifndef __CLOCK_SM_ALARM_H__
#define __CLOCK_SM_ALARM_H__

#include "sm.h"

enum sm_alarm_ss {
  SM_ALARM_HIT_ALARM0     = 0, //一般闹钟到时间
  SM_ALARM_HIT_ALARM1     = 1, // 整点报时到时间
};

void sm_alarm(unsigned char from, unsigned char to, enum task_events);
#endif