#ifndef __CLOCK_SM_CLOCK_MOD_ALARM_H__
#define __CLOCK_SM_CLOCK_MOD_ALARM_H__

#include "sm.h"

enum sm_clock_mod_alarm_ss {
  SM_CLOCK_MODIFY_ALARM_INIT = 0, // ��ʼ״̬
  SM_CLOCK_MODIFY_ALARM_HH   = 1, // �޸�Alarmʱ
  SM_CLOCK_MODIFY_ALARM_MM   = 2, // �޸�Alarm��
  SM_CLOCK_MODIFY_ALARM_DAY1 = 3,  // �޸�Alarm����һ����״̬
  SM_CLOCK_MODIFY_ALARM_DAY2 = 4,  // �޸�Alarm���ڶ�����״̬
  SM_CLOCK_MODIFY_ALARM_DAY3 = 5,  // �޸�Alarm����������״̬
  SM_CLOCK_MODIFY_ALARM_DAY4 = 6,  // �޸�Alarm�����Ŀ���״̬
  SM_CLOCK_MODIFY_ALARM_DAY5 = 7,  // �޸�Alarm�����忪��״̬
  SM_CLOCK_MODIFY_ALARM_DAY6 = 8,  // �޸�Alarm����������״̬
  SM_CLOCK_MODIFY_ALARM_DAY7 = 9  // �޸�Alarm�����տ���״̬  
};

extern const char * code sm_clock_mod_alarm_ss_name[];

void sm_clock_mod_alarm(unsigned char from, unsigned char to, enum task_events);

#endif