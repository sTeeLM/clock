#ifndef __CLOCK_TASK_H__
#define __CLOCK_TASK_H__

enum task_events
{
  EV_250MS            = 0, // ��Լÿ250msתһ��
  EV_1S               = 1, // ��Լÿ1sתһ��  
  EV_SCAN_KEY         = 2, // ɨ�谴��
  EV_KEY_MOD_DOWN     = 3, // mod������
  EV_KEY_MOD_UP       = 4, // mod��̧��
  EV_KEY_MOD_PRESS    = 5, // mod���̰�
  EV_KEY_MOD_LPRESS   = 6, // mod������  
  EV_KEY_SET_DOWN     = 7, // set������
  EV_KEY_SET_UP       = 8, // set��̧��
  EV_KEY_SET_PRESS    = 9, // set���̰�
  EV_KEY_SET_LPRESS   = 10, // set������
  EV_KEY_MOD_SET_PRESS    = 11, // mod set��ͬʱ�̰�
  EV_KEY_MOD_SET_LPRESS   = 12, // mod set ��ͬʱ���� 
  EV_ALARM_TEST       = 13, // // �������𣬵��ǲ�֪������һ��  
  EV_ALARM0           = 14, // ����0Ӧ������
  EV_ALARM1           = 15, // ����1Ӧ������
  EV_COUNTER          = 16, // ��ʱ����ʱ��
  EV_POWER_SAVE       = 17, // Ӧ�ý���PS״̬ 
  EV_COUNT  
};

extern unsigned int idata ev_bits0;
extern unsigned int idata ev_bits1;

typedef void (code *TASK_PROC)(enum task_events);

void task_initialize (void);


// ��Щ��Ҳ���ж��ﱻ���ã����Բ����Ǵ�����������ְ���ú�ʵ����
#define set_task(ev1)             \
  do{                             \
    if(ev1 < 16)                  \
      ev_bits0 |= 1<<ev1;         \
    else                          \
      ev_bits1 |= 1<<(ev1 - 16);  \
  }while(0)

#define clr_task(ev1)               \
  do{                               \
    if(ev1 < 16)                    \
      ev_bits0 &= ~(1<<ev1);        \
    else                            \
      ev_bits1 &= ~(1<<(ev1 - 16)); \  
  }while(0)
    
#define  test_task(ev1)             \
  (ev1 < 16 ? (ev_bits0 & (1<<ev1)) : (ev_bits1 & (1<<(ev1 - 16))))

void run_task(void);

#endif