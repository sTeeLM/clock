#ifndef __CLOCK_SM_POWERPACK_DISPLAY_H__
#define __CLOCK_SM_POWERPACK_DISPLAY_H__

#include "sm.h"

enum sm_power_pack_display_ss {
  SM_POWER_PACK_DISPLAY_INIT     = 0, //初始状态
  SM_POWER_PACK_DISPLAY_POWER    = 1, //电量显示
};

extern const char * code sm_power_pack_display_ss_name[];

void sm_power_pack_display_init(unsigned char from, unsigned char to, enum task_events);
void sm_power_pack_display_submod0(unsigned char from, unsigned char to, enum task_events);

#endif
