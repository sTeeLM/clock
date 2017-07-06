#include <string.h>

#include "task.h"
#include "sm.h"

/* hardware*/
#include "clock.h"
#include "rtc.h"
#include "key.h"
#include "led.h"
#include "alarm.h"
#include "power.h"
#include "timer.h"
#include "fuse.h"
#include "int_hub.h"
#include "hg.h"
#include "thermo.h"
#include "gyro.h"
#include "tripwire.h"

#include "debug.h"


/*
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
*/

static const TASK_PROC code task_procs[EV_COUNT] = 
{
  /* EV_250MS            = 0, // ��Լÿ250msתһ�� */
  null_proc,
  /* EV_1S               = 1, // ��Լÿ1sתһ��   */
  null_proc,
  /* EV_SCAN_KEY         = 2, // ɨ�谴�� */
  scan_key_proc,
  /* EV_KEY_MOD_XX */
  mod_proc,
  mod_proc,
  mod_proc,
  mod_proc,
  /* EV_KEY_SET_XX */
  set_proc,
  set_proc,
  set_proc,
  set_proc,
  /* EV_KEY_MOD_SET_XX */
  mod_set_proc,
  mod_set_proc, 
  /* EV_SCAN_INT_HUB     = 13, // ɨ��fuse��hg��gyro */
  scan_int_hub_proc,
  /* EV_FUSE_XX */
  fuse_proc,
  fuse_proc,
  fuse_proc,
  fuse_proc,
  /* EV_ROTATE_HG */
	null_proc,
  /* EV_ROTATE_GYRO */
  null_proc,
  /* EV_ACC_GYRO */
  null_proc,
  /* EV_DROP_GYRO */
  null_proc,
  /* EV_THERMO_XX */
  thermo_proc,
  thermo_proc,
  /* EV_TRIPWIRE         = 24, // tripwire������ */
  tripwire_proc,
  /* EV_FUSE_SEL_XX */
  null_proc,
  null_proc,
  /* EV_ALARM_XX */
  alarm_proc,
  alarm_proc,
  /* EV_COUNTER          = 29, // ��ʱ����ʱ�� */
  null_proc,
  /* EV_POWER_SAVE       = 30, // Ӧ�ý���PS״̬ */
  power_proc  
};


unsigned int idata ev_bits0;
unsigned int idata ev_bits1;

void task_initialize (void)
{
  CDBG("task_initialize\n");
  ev_bits0 = 0;
  ev_bits1 = 0;
}

void run_task()
{
  unsigned char c;
  while(1) {
    for(c = 0; c < EV_COUNT; c++) {
      if(test_task(c)) {
        task_procs[c](c);
        clr_task(c);
      }
    }
  }
}


