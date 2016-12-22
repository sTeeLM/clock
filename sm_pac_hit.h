#ifndef __CLOCK_SM_PAC_HIT_H__
#define __CLOCK_SM_PAC_HIT_H__

#include "sm.h"

enum sm_pac_hit_ss {
  SM_PAC_HIT_ALARM0     = 0, //һ�����ӵ�ʱ��
  SM_PAC_HIT_ALARM1     = 1, // ���㱨ʱ��ʱ��
  SM_PAC_HIT_COUNTER    = 2, //��ʱ����ʱ��
  SM_PAC_HIT_POWERSAVE  = 3, //����ڵ�ģʽ
};

void sm_pac_hit(unsigned char from, unsigned char to, enum task_events);

#endif