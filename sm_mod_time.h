#ifndef __CLOCK_SM_MOD_TIME_H__
#define __CLOCK_SM_MOD_TIME_H__

#include "sm.h"

enum sm_mod_time_ss {
  SM_MODIFY_TIME_INIT = 0, // ��ʼ״̬
  SM_MODIFY_TIME_HH   = 1, // �޸�ʱ
  SM_MODIFY_TIME_MM   = 2, // �޸ķ�
  SM_MODIFY_TIME_SS   = 3, // �޸ķ�  
  SM_MODIFY_TIME_YY   = 4, // �޸���
  SM_MODIFY_TIME_MO   = 5, // �޸���
  SM_MODIFY_TIME_DD   = 6, // �޸���   
};

void sm_mod_time(unsigned char from, unsigned char to, enum task_events);

#endif