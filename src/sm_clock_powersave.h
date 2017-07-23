#ifndef __CLOCK_SM_CLOCK_POWERSAVE_H__
#define __CLOCK_SM_CLOCK_POWERSAVE_H__

#include "sm.h"

enum sm_clock_powersave_ss {
  SM_CLOCK_POWERSAVE_INIT     = 0, // ³õÊ¼×´Ì¬
  SM_CLOCK_POWERSAVE_PS       = 1, // ½Úµç×´Ì¬
};

void sm_clock_powersave(unsigned char from, unsigned char to, enum task_events);

#endif