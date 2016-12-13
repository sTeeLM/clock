#include <string.h>

#include "task.h"
#include "sm.h"

/* hardware*/
#include "timer.h"
#include "rtc.h"
#include "key.h"
#include "led.h"
#include "alarm.h"
#include "power.h"
#include "counter.h"
#include "debug.h"

/*
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
*/

static const TASK_PROC code task_procs[EV_COUNT] = 
{
  scan_key_proc,
  mod_proc,
  mod_proc,
  mod_proc,
  mod_proc,  
  set_proc,
  set_proc,
  set_proc,
  set_proc,
  mod_set_proc,
  mod_set_proc,  
  alarm_proc,
  counter_proc,
  power_proc,
  state_machine_timer_proc,
  state_machine_timer_proc  
};


unsigned int idata ev_bits;

void task_initialize (void)
{
  CDBG("task_initialize\n");
  ev_bits = 0;
}

void run_task(void)
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

