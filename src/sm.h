#ifndef __CLOCK_SM_H__
#define __CLOCK_SM_H__

#include "task.h"

// max 16
enum sm_states_clock
{
  SM_CLOCK_DISPLAY      = 0, // һ����ʾ״̬
  SM_CLOCK_MODIFY_TIME  = 1, // �޸�ʱ��
  SM_CLOCK_MODIFY_ALARM = 2, // �޸�����
  SM_CLOCK_MODIFY_GLOBAL_FLAG = 3, // �޸�ȫ��״̬
  SM_CLOCK_POWERSAVE    = 4, // �ڵ�
  SM_CLOCK_ALARM        = 5, // ����/���㱨ʱ
  SM_CLOCK_TIMER        = 6, // �����
  SM_CLOCK_COUNTER      = 7, // ����ʱ����
};

// max 16
enum sm_stats_fuse
{
  SM_FUSE_TEST          = 0, // fuse����
  SM_FUSE_MODE          = 1, // fuseģʽѡ��
  SM_FUSE_PARAM         = 2,// fuse��������
  SM_FUSE_TIMER         = 3,// fuse��ʱģʽ
  SM_FUSE_GRENADE       = 4,// fuse����ģʽ
  SM_FUSE_DETONATE      = 5,// boom!
  SM_FUSE_POWERSAVE     = 6,// �ڵ�
};

// max 256
enum sm_tables
{
  SM_CLOCK  = 0,
  SM_FUSE   = 1,
  SM_TABLE_CNT
};

typedef void (code *SM_PROC)(unsigned char from, unsigned char to, enum task_events ev);

extern const char * code sm_tables_name[];
extern const char * * code sm_states_name[];

struct sm_trans
{
  unsigned char from_table;  
  unsigned char from_state;
  enum task_events event;
  unsigned char to_table;
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