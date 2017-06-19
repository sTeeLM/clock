#ifndef __CLOCK_SM_PAC_HIT_H__
#define __CLOCK_SM_PAC_HIT_H__

#include "sm.h"

enum sm_powersave_ss {
  SM_POWERSAVE_INIT     = 0, // ��ʼ״̬
  SM_POWERSAVE_PS       = 1, // �ڵ�״̬
};

void sm_powersave(unsigned char from, unsigned char to, enum task_events);

#endif