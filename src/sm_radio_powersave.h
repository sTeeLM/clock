#ifndef __CLOCK_SM_RADIO_POWERSAVE_H__
#define __CLOCK_SM_RADIO_POWERSAVE_H__

#include "sm.h"

enum sm_radio_powersave_ss {
  SM_RADIO_POWERSAVE_INIT      = 0, // 初始状态
  SM_RADIO_POWERSAVE_SLEEP     = 1, // 睡眠状态
  
};

extern const char * code sm_radio_powersave_ss_name[];

void sm_radio_powersave_init(unsigned char from, unsigned char to, enum task_events);
void sm_radio_powersave_submod0(unsigned char from, unsigned char to, enum task_events);

#endif
