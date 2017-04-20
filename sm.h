#ifndef __CLOCK_SM_H__
#define __CLOCK_SM_H__

#include "task.h"

enum sm_states
{
  SM_DISPLAY            = 0, // һ����ʾ״̬
  SM_MODIFY_TIME        = 1, // �޸�ʱ��
  SM_MODIFY_ALARM       = 2, // �޸�����
  SM_MODIFY_GLOBAL_FLAG = 3, // �޸�ȫ��״̬
  SM_PAC_HIT            = 4, // �ڵ�/�ֱ�/����ʱ��ʱ��
  SM_TIMER              = 5, // �����
  SM_COUNTER            = 6, // ����ʱ����
  SM_FUSE_TEST          = 7, // fuse����
  SM_FUSE_MODE          = 8, // fuseģʽѡ��
  SM_FUSE_PARAM         = 9, // fuse��������
  SM_FUSE_TIMER         = 10,// fuse��ʱģʽ
  SM_FUSE_GRENADE       = 11,// fuse����ģʽ
  SM_FUSE_DETONATE      = 12,// boom!
  SM_FUSE_POWERSAVE     = 13,// ʡ��ģʽ
};


typedef void (code *SM_PROC)(unsigned char from, unsigned char to, enum task_events ev);

struct sm_trans
{
  unsigned char from_state;
  enum task_events event;
  unsigned char to_state;
  SM_PROC sm_proc;
};


#define get_sm_state(st) \
  (((st & 0xF0) >> 4) & 0x0F)

#define get_sm_ss_state(st) \
  (st & 0x0F)

void run_state_machine(enum task_events);
void state_machine_timer_proc(enum task_events ev);
void sm_initialize (void);

#endif