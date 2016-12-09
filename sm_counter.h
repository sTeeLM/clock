#ifndef __CLOCK_SM_COUNTER_H__
#define __CLOCK_SM_COUNTER_H__

#include "task.h"

enum sm_counter_ss {
  SM_COUNTER_INIT       = 0, // ��ʼ״̬
  SM_COUNTER_MODIFY_HH  = 1, // ����ʱ
  SM_COUNTER_MODIFY_MM  = 2, // ���÷�
  SM_COUNTER_MODIFY_SS  = 3, // ������
  SM_COUNTER_RUNNING    = 4, // ����ʱ״̬
  SM_COUNTER_STOP       = 5, // ֹͣ/��ͣ״̬
};

void sm_counter(unsigned char from, unsigned char to, enum task_events);

#endif