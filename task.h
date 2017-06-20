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
  EV_SCAN_INT_HUB     = 13, // ɨ��fuse��hg��gyro
  EV_FUSE0_SHORT      = 14, // fuse0����·
  EV_FUSE0_BROKE      = 15, // fuse0������
  EV_FUSE1_SHORT      = 16, // fuse0����·
  EV_FUSE1_BROKE      = 17, // fuse0������
  EV_ROTATE_HG        = 18, // hg������б״̬�ı�
  EV_ROTATE_GYRO      = 19, // gyro������б״̬�ı�	
  EV_ACC_GYRO         = 20, // gyro �������ٶȸı�
  EV_THERMO_HI     = 21, // �¶�̫��
  EV_THERMO_LO     = 22, // �¶�̫��
  EV_FUSE0_TRIGGER    = 23, // fuse0Ӧ�ñ����� 
  EV_FUSE1_TRIGGER    = 24, // fuse1Ӧ�ñ�����
  EV_TRIPWIRE         = 25, // tripwire������
  EV_FUSE_SEL0         = 26, // fuse �����¼�0
  EV_FUSE_SEL1         = 27, // fuse �����¼�1
  EV_ALARM0           = 28, // ����0Ӧ������
  EV_ALARM1           = 29, // ����1Ӧ������
  EV_COUNTER          = 30, // ��ʱ����ʱ��
  EV_POWER_SAVE       = 31, // Ӧ�ý���PS״̬ 
  EV_COUNT  
};

extern unsigned int idata ev_bits0;
extern unsigned int idata ev_bits1;

typedef void (code *TASK_PROC)(enum task_events);

void task_initialize (void);


// ��Щ��Ҳ���ж��ﱻ���ã����Բ����Ǵ�����������ֻ���ú�ʵ����
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