#ifndef __CLOCK_TASK_H__
#define __CLOCK_TASK_H__

enum task_events
{
  EV_SCAN_KEY         = 0, // ɨ�谴��
  EV_KEY_MOD_DOWN     = 1, // mod������
  EV_KEY_MOD_UP       = 2, // mod��̧��
  EV_KEY_MOD_PRESS    = 3, // mod���̰�
  EV_KEY_MOD_LPRESS   = 4, // mod������  
  EV_KEY_SET_DOWN     = 5, // set������
  EV_KEY_SET_UP       = 6, // set��̧��
  EV_KEY_SET_PRESS    = 7, // set���̰�
  EV_KEY_SET_LPRESS   = 8, // set������
  EV_KEY_MOD_SET_PRESS    = 9, // mod set��ͬʱ�̰�
  EV_KEY_MOD_SET_LPRESS   = 10, // mod set ��ͬʱ����    
  EV_ALARM            = 11, // ����Ӧ������
  EV_COUNTER          = 12, // ��ʱ����ʱ��
  EV_POWER_SAVE       = 13, // Ӧ�ý���PS״̬ 
  EV_250MS            = 14, // ��Լÿ250msתһ��
  EV_1S               = 15, // ��Լÿ1sתһ��
  EV_COUNT  
};

enum sm_states
{
  SM_DISPLAY            = 0, // һ����ʾ״̬
  SM_MODIFY_TIME        = 1, // �޸�ʱ��
  SM_MODIFY_ALARM       = 2, // �޸�����
  SM_MODIFY_GLOBAL_FLAG = 3, // �޸�ȫ��״̬
  SM_PAC_HIT            = 4, // �ڵ�/�ֱ�/����ʱ��ʱ��
  SM_TIMER              = 5, // �����
  SM_COUNTER            = 6  // ����ʱ����
};

extern unsigned int ev_bits;

typedef void (*TASK_PROC)(enum task_events);
typedef void (*SM_PROC)(unsigned char from, unsigned char to, enum task_events ev);

struct sm_trans
{
  unsigned char from_state;
  unsigned char event;
  unsigned char to_state;
  SM_PROC sm_proc;
};

void task_initialize (void);

#define set_task(ev) \
  do{ev_bits |= 1<<ev;}while(0)

#define clr_task(ev) \
  do{ev_bits &= ~(1<<ev);}while(0)
    
#define  test_task(ev) \
  (ev_bits & (1<<ev))

#define get_sm_state(state) \
  (((state & 0xF0) >> 4) & 0x0F)

#define get_sm_ss_state(state) \
  (state & 0x0F)

void run_task(void);

void state_machine_proc(enum task_events);
  
void run_state_machine(enum task_events);

#endif