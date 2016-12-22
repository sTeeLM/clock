#ifndef __CLOCK_SM_PAC_HIT_H__
#define __CLOCK_SM_PAC_HIT_H__

#include "sm.h"

enum sm_pac_hit_ss {
  SM_PAC_HIT_ALARM0     = 0, //一般闹钟到时间
  SM_PAC_HIT_ALARM1     = 1, // 整点报时到时间
  SM_PAC_HIT_COUNTER    = 2, //计时器到时间
  SM_PAC_HIT_POWERSAVE  = 3, //进入节电模式
};

void sm_pac_hit(unsigned char from, unsigned char to, enum task_events);

#endif