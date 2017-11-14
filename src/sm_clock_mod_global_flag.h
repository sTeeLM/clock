#ifndef __CLOCK_SM_CLOCK_MOD_GLOBAL_FLAG_H__
#define __CLOCK_SM_CLOCK_MOD_GLOBAL_FLAG_H__

#include "sm.h"

enum sm_clock_mod_global_flag_ss {
  SM_CLOCK_MODIFY_GLOBAL_FLAG_INIT   = 0, // ��ʼ״̬
  SM_CLOCK_MODIFY_GLOBAL_FLAG_PS     = 1, // �ڵ�ģʽ��ʱʱ���л���0��15��30
  SM_CLOCK_MODIFY_GLOBAL_FLAG_BS     = 2, // ���㱨ʱON/OFF�л�
  SM_CLOCK_MODIFY_GLOBAL_FLAG_ALARM_MUSIC = 3, // ������������ 
  SM_CLOCK_MODIFY_GLOBAL_FLAG_BEEP   = 4, // ������
  SM_CLOCK_MODIFY_GLOBAL_FLAG_1224   = 5, // 12/24Сʱ��ʾ�л�
};

extern const char * code sm_clock_mod_global_flag_name[];

void sm_clock_mod_global_flag(unsigned char from, unsigned char to, enum task_events);

#endif