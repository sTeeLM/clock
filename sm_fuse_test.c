#include "sm_fuse_test.h"
#include "debug.h"
#include "mod_common.h"
#include "alarm.h"
#include "lt_timer.h"

void sm_fuse_test(unsigned char from, unsigned char to, enum task_events ev)
{
  CDBG("sm_fuse_test %bd %bd %bd\n", from, to, ev);

  // ��mod1������ʾʱ���ģʽ
  if(get_sm_ss_state(to) == SM_FUSE_TEST_INIT && ev == EV_KEY_MOD_SET_LPRESS) {
    alarm_switch_off();
    lt_timer_switch_on();
    display_logo(10);
    return;
  }
  
  // �л���ʱ����ʾ��ģʽ
  if(get_sm_ss_state(from) == SM_FUSE_TEST_INIT 
    && get_sm_ss_state(to) == SM_FUSE_TEST_FUSE0_SHORT
    && ev == EV_KEY_MOD_UP) {
    
    return;
  }
}