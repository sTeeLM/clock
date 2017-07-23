#ifndef __CLOCK_SM_CLOCK_COUNTER_H__
#define __CLOCK_SM_CLOCK_COUNTER_H__

#include "sm.h"

enum sm_clock_counter_ss {
  SM_CLOCK_COUNTER_INIT       = 0, // ��ʼ״̬
  SM_CLOCK_COUNTER_MODIFY_HH  = 1, // ����ʱ
  SM_CLOCK_COUNTER_MODIFY_MM  = 2, // ���÷�
  SM_CLOCK_COUNTER_MODIFY_SS  = 3, // ������
  SM_CLOCK_COUNTER_RUNNING    = 4, // ����ʱ״̬
  SM_CLOCK_COUNTER_PAUSE      = 5, // ��ͣ״̬
  SM_CLOCK_COUNTER_STOP       = 6, // ֹͣ/����״̬
};

void sm_clock_counter(unsigned char from, unsigned char to, enum task_events);

#endif