#ifndef __CLOCK_SM_CLOCK_POWERSAVE_H__
#define __CLOCK_SM_CLOCK_POWERSAVE_H__

#include "sm.h"

enum sm_clock_powersave_ss {
  SM_CLOCK_POWERSAVE_INIT     = 0, // ��ʼ״̬
  SM_CLOCK_POWERSAVE_PS       = 1, // �ڵ�״̬
};

extern const char * code sm_clock_powersave_ss_name[];

void sm_clock_powersave(unsigned char from, unsigned char to, enum task_events);

#endif