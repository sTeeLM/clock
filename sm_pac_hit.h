#ifndef __CLOCK_SM_PAC_HIT_H__
#define __CLOCK_SM_PAC_HIT_H__

#include "sm.h"

enum sm_pac_hit_ss {
  SM_PAC_HIT_ALARM      = 0, //闹钟到时间
  SM_PAC_HIT_COUNTER    = 1, //计时器到时间
  SM_PAC_HIT_POWERSAVE  = 2, //进入节电模式
};

void sm_pac_hit(unsigned char from, unsigned char to, enum task_events);

#endif