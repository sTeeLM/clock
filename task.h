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

extern unsigned int idata ev_bits;

typedef void (code *TASK_PROC)(enum task_events);

void task_initialize (void);

#define set_task(ev1) \
  do{ev_bits |= 1<<ev1;}while(0)

#define clr_task(ev1) \
  do{ev_bits &= ~(1<<ev1);}while(0)
    
#define  test_task(ev1) \
  (ev_bits & (1<<ev1))

void run_task(void);

#endif