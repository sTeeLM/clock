#ifndef __CLOCK_SM_DISPLAY_H__
#define __CLOCK_SM_DISPLAY_H__

#include "sm.h"

enum sm_display_ss {
  SM_DISPLAY_INIT   = 0, // ��ʼ״̬
  SM_DISPLAY_HHMMSS = 1, // ��ʾʱ����
  SM_DISPLAY_YYMMDD = 2, // ��ʾ������
  SM_DISPLAY_WEEK   = 3, // ��ʾ���ڼ�
  SM_DISPLAY_TEMP   = 4, // ��ʾ�¶�
};

void sm_display(unsigned char from, unsigned char to, enum task_events);

#endif