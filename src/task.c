#include <string.h>

#include "task.h"
#include "sm.h"

/* hardware*/
#include "com.h"
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

#include "shell.h"
#include "debug.h"


/*
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
  EV_FUSE_SEL2         = 27, // fuse �����¼�2  
  EV_ALARM0           = 28, // ����0Ӧ������
  EV_ALARM1           = 29, // ����1Ӧ������
  EV_COUNTER          = 30, // ��ʱ����ʱ��
  EV_POWER_SAVE       = 31, // Ӧ�ý���PS״̬    
*/

const char * task_name[] =
{
  "EV_250MS",
  "EV_1S",
  "EV_SCAN_KEY",
  "EV_KEY_SET_DOWN",
  "EV_KEY_SET_PRESS",
  "EV_KEY_SET_LPRESS",
  "EV_KEY_SET_UP",
  "EV_KEY_MOD_DOWN",
  "EV_KEY_MOD_PRESS",
  "EV_KEY_MOD_LPRESS",
  "EV_KEY_MOD_UP",
  "EV_KEY_MOD_SET_PRESS",
  "EV_KEY_MOD_SET_LPRESS",
  "EV_SCAN_INT_HUB",
  "EV_FUSE0_SHORT",
  "EV_FUSE0_BROKE",
  "EV_FUSE1_SHORT",
  "EV_FUSE1_BROKE",
  "EV_ROTATE_HG",
  "EV_ROTATE_GYRO",
  "EV_ACC_GYRO",
  "EV_DROP_GYRO",
  "EV_THERMO_HI",
  "EV_THERMO_LO",
  "EV_TRIPWIRE",
  "EV_FUSE_SEL0",
  "EV_FUSE_SEL1",
  "EV_FUSE_SEL2",
  "EV_ALARM0",
  "EV_ALARM1",
  "EV_COUNTER",
  "EV_POWER_SAVE",
};

static const TASK_PROC code task_procs[EV_COUNT] = 
{
  /* EV_250MS            = 0, // ��Լÿ250msתһ�� */
  null_proc,
  /* EV_1S               = 1, // ��Լÿ1sתһ��   */
  null_proc,
  /* EV_SCAN_KEY         = 2, // ɨ�谴�� */
  scan_key_proc,
  /* EV_KEY_SET_XX */
  set_proc,
  set_proc,
  set_proc,
  set_proc,
  /* EV_KEY_MOD_XX */
  mod_proc,
  mod_proc,
  mod_proc,
  mod_proc,
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
  null_proc,
  /* EV_ALARM_XX */
  alarm_proc,
  alarm_proc,
  /* EV_COUNTER          = 30, // ��ʱ����ʱ�� */
  null_proc,
  /* EV_POWER_SAVE       = 31, // Ӧ�ý���PS״̬ */
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
  
again:  
  while(com_try_get_key() == 0) {
    for(c = 0; c < EV_COUNT; c++) {
      if(test_task(c)) {
        clr_task(c);
        task_procs[c](c);
      }
    }
  }
  
  run_shell();
  
  goto again;
}


