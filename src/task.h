#ifndef __CLOCK_TASK_H__
#define __CLOCK_TASK_H__

// max 32
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
  EV_FUSE0_BROKE      = 14, // fuse0������
  EV_FUSE1_BROKE      = 15, // fuse0������
  EV_ROTATE_HG        = 16, // hg������б״̬�ı�
  EV_ROTATE_GYRO      = 17, // gyro������б״̬�ı�	
  EV_ACC_GYRO         = 18, // gyro �����ζ�
  EV_DROP_GYRO         = 19, // gyro �������䣨ʧ�أ�
  EV_THERMO_HI     = 20, // �¶�̫��
  EV_THERMO_LO     = 21, // �¶�̫��
  EV_TRIPWIRE         = 22, // tripwire������
  EV_FUSE_SEL0         = 23, // fuse �����¼�0
  EV_FUSE_SEL1         = 24, // fuse �����¼�1
  EV_FUSE_SEL2         = 25, // fuse �����¼�2  
  EV_ALARM0           = 26, // ����0Ӧ������
  EV_ALARM1           = 27, // ����1Ӧ������
  EV_COUNTER          = 28, // ��ʱ����ʱ��
  EV_POWER_SAVE       = 29, // Ӧ�ý���PS״̬ 
  EV_COUNT  
};

extern unsigned int idata ev_bits0;
extern unsigned int idata ev_bits1;

extern const char * code task_name[];

typedef void (code *TASK_PROC)(enum task_events);

void task_initialize (void);

void task_dump(void);

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