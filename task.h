#ifndef __CLOCK_TASK_H__
#define __CLOCK_TASK_H__

enum task_events
{
  EV_250MS            = 0, // ��Լÿ250msתһ��
  EV_1S               = 1, // ��Լÿ1sתһ��  
  EV_SCAN_KEY         = 2, // ɨ�谴�� 
  EV_KEY_SET_DOWN     = 3, // set������
  EV_KEY_SET_PRESS    = 4, // set���̰�
  EV_KEY_SET_LPRESS   = 5, // set������
  EV_KEY_SET_UP       = 6, // set��̧��
  EV_KEY_MOD_DOWN     = 7, // mod������
  EV_KEY_MOD_PRESS    = 8, // mod���̰�
  EV_KEY_MOD_LPRESS   = 9, // mod������
  EV_KEY_MOD_UP       = 10, // mod��̧�� 
  EV_KEY_MOD_SET_PRESS    = 11, // mod set ��ͬʱ�̰�
  EV_KEY_MOD_SET_LPRESS   = 12, // mod set ��ͬʱ���� 
  EV_SCAN_INT_HUB     = 13, // ɨ��fuse��hg��gyro
  EV_FUSE0_SHORT      = 14, // fuse0����·
  EV_FUSE0_BROKE      = 15, // fuse0������
  EV_FUSE1_SHORT      = 16, // fuse0����·
  EV_FUSE1_BROKE      = 17, // fuse0������
  EV_ROTATE_HG        = 18, // hg������б״̬�ı�
  EV_ROTATE_GYRO      = 19, // gyro������б״̬�ı�	
  EV_ACC_GYRO         = 20, // gyro �����ζ�
  EV_DROP_GYRO         = 21, // gyro �������䣨ʧ�أ�
  EV_THERMO_HI     = 22, // �¶�̫��
  EV_THERMO_LO     = 23, // �¶�̫��
  EV_TRIPWIRE         = 24, // tripwire������
  EV_FUSE_SEL0         = 25, // fuse �����¼�0
  EV_FUSE_SEL1         = 26, // fuse �����¼�1
  EV_ALARM0           = 27, // ����0Ӧ������
  EV_ALARM1           = 28, // ����1Ӧ������
  EV_COUNTER          = 29, // ��ʱ����ʱ��
  EV_POWER_SAVE       = 30, // Ӧ�ý���PS״̬ 
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