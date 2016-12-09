#ifndef __CLOCK_SM_MOD_ALARM_H__
#define __CLOCK_SM_MOD_ALARM_H__

#include "task.h"

enum sm_mod_alarm_ss {
  SM_MODIFY_ALARM_INIT= 0, // ��ʼ״̬
  SM_MODIFY_ALARM0_HH = 1, // �޸�Alarm 0ʱ
  SM_MODIFY_ALARM0_MM = 2, // �޸�Alarm 0��
  SM_MODIFY_ALARM0_ON = 3, // �޸�Alarm 0��  
  SM_MODIFY_ALARM1_HH = 4, // �޸�Alarm 1ʱ
  SM_MODIFY_ALARM1_MM = 5, // �޸�Alarm 1��
  SM_MODIFY_ALARM1_ON = 6, // �޸�Alarm 1��  
};

void sm_mod_alarm(unsigned char from, unsigned char to, enum task_events);

#endif