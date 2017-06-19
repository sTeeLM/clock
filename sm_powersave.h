#ifndef __CLOCK_SM_PAC_HIT_H__
#define __CLOCK_SM_PAC_HIT_H__

#include "sm.h"

enum sm_powersave_ss {
  SM_POWERSAVE_INIT     = 0, // ³õÊ¼×´Ì¬
  SM_POWERSAVE_PS       = 1, // ½Úµç×´Ì¬
};

void sm_powersave(unsigned char from, unsigned char to, enum task_events);

#endif