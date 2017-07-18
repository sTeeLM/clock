#ifndef __CLOCK_SM_FUSE_GRENADE_H__
#define __CLOCK_SM_FUSE_GRENADE_H__

#include "sm.h"

enum sm_fuse_grenade_ss {
  SM_FUSE_GRENADE_INIT        = 0, // ��ʼ״̬
  SM_FUSE_GRENADE_PREARMED    = 1, // Ԥ������ģʽ
  SM_FUSE_GRENADE_ARMED       = 2, // ������ģʽ
  SM_FUSE_GRENADE_DISARMED    = 3, // ���״̬
  SM_FUSE_GRENADE_PREDETONATE = 4  // ԤDETONATE
};

void sm_fuse_grenade(unsigned char from, unsigned char to, enum task_events);

#endif