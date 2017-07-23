#ifndef __CLOCK_SM_FUSE_TIMER_H__
#define __CLOCK_SM_FUSE_TIMER_H__

#include "sm.h"

enum sm_fuse_timer_ss {
  SM_FUSE_TIMER_INIT      = 0, // ��ʼ״̬
  SM_FUSE_TIMER_PREARMED  = 1, // ��ʼ�������ò���
  SM_FUSE_TIMER_ARMED     = 2, // ���armed�ˣ�
  SM_FUSE_TIMER_VERIFY    = 3, // ���password
  SM_FUSE_TIMER_DISARMED  = 4, // �������
  SM_FUSE_TIMER_PREDETONATE = 5, // ׼��detonate
};

void sm_fuse_timer(unsigned char from, unsigned char to, enum task_events);

#endif