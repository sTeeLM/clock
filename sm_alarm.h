#ifndef __CLOCK_SM_ALARM_H__
#define __CLOCK_SM_ALARM_H__

#include "sm.h"

enum sm_alarm_ss {
  SM_ALARM_HIT_ALARM0     = 0, //һ�����ӵ�ʱ��
  SM_ALARM_HIT_ALARM1     = 1, // ���㱨ʱ��ʱ��
};

void sm_alarm(unsigned char from, unsigned char to, enum task_events);
#endif