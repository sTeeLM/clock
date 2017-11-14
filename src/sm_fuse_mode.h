#ifndef __CLOCK_SM_FUSE_MODE_H__
#define __CLOCK_SM_FUSE_MODE_H__

#include "sm.h"

enum sm_fuse_mode_ss {
  SM_FUSE_MODE_INIT      = 0, // ��ʼ״̬
  SM_FUSE_MODE_TIMER     = 1, // ��ʱ����ģʽ
  SM_FUSE_MODE_GRENADE   = 2, // ��ײ��������
};

extern const char * code sm_fuse_mode_ss_name[];

void sm_fuse_mode(unsigned char from, unsigned char to, enum task_events);

#endif