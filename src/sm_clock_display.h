#ifndef __CLOCK_SM_CLOCK_DISPLAY_H__
#define __CLOCK_SM_CLOCK_DISPLAY_H__

#include "sm.h"

enum sm_clock_display_ss {
  SM_CLOCK_DISPLAY_INIT   = 0, // ��ʼ״̬
  SM_CLOCK_DISPLAY_HHMMSS = 1, // ��ʾʱ����
  SM_CLOCK_DISPLAY_YYMMDD = 2, // ��ʾ������
  SM_CLOCK_DISPLAY_WEEK   = 3, // ��ʾ���ڼ�
  SM_CLOCK_DISPLAY_TEMP   = 4, // ��ʾ�¶�
};

extern const char * code sm_clock_display_ss_name[];

void sm_clock_display(unsigned char from, unsigned char to, enum task_events);

#endif