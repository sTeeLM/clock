#ifndef __CLOCK_SM_H__
#define __CLOCK_SM_H__

#include "task.h"

enum sm_states
{
  SM_CLOCK_DISPLAY      = 0, // һ����ʾ״̬
  SM_CLOCK_MODIFY_TIME        = 1, // �޸�ʱ��
  SM_CLOCK_MODIFY_ALARM = 2, // �޸�����
  SM_CLOCK_MODIFY_GLOBAL_FLAG = 3, // �޸�ȫ��״̬
  SM_CLOCK_POWERSAVE    = 4, // �ڵ�
  SM_CLOCK_ALARM        = 5, // ����/���㱨ʱ
  SM_CLOCK_TIMER        = 6, // �����
  SM_CLOCK_COUNTER      = 7, // ����ʱ����
  SM_FUSE_TEST          = 8, // fuse����
  SM_FUSE_MODE          = 9, // fuseģʽѡ��
  SM_FUSE_PARAM         = 10,// fuse��������
  SM_FUSE_TIMER         = 11,// fuse��ʱģʽ
  SM_FUSE_GRENADE       = 12,// fuse����ģʽ
  SM_FUSE_DETONATE      = 13,// boom!
  SM_FUSE_POWERSAVE     = 14,// �ڵ�
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
void null_proc(enum task_events ev);
void sm_initialize (void);

#endif