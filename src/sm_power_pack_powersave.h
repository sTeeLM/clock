#ifndef __CLOCK_SM_POWER_PACK_POWERSAVE_H__
#define __CLOCK_SM_POWER_PACK_POWERSAVE_H__

#include "sm.h"

enum sm_power_pack_powersave_ss {
  SM_POWER_PACK_POWERSAVE_INIT      = 0, // 初始状态
  SM_POWER_PACK_POWERSAVE_SLEEP     = 1, // 睡眠状态
  
};

extern const char * code sm_power_pack_powersave_ss_name[];

void sm_power_pack_powersave_init(unsigned char from, unsigned char to, enum task_events);
void sm_power_pack_powersave_submod0(unsigned char from, unsigned char to, enum task_events);

#endif