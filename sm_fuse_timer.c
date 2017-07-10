#include "sm_fuse_timer.h"
#include "debug.h"
#include "mod_common.h"
#include "led.h"
#include "lt_timer.h"

void sm_fuse_timer(unsigned char from, unsigned char to, enum task_events ev)
{
  CDBG("sm_fuse_timer %bd %bd %bd\n", from, to, ev);
  
  if(get_sm_ss_state(to) == SM_FUSE_TIMER_INIT) {
    display_logo(DISPLAY_LOGO_TYPE_FUSE, 3);
    return;
  }
  
  // ��1�����armed״̬
  if(get_sm_ss_state(to) == SM_FUSE_TIMER_ARMED && ev == EV_1S) {
    
    return;
  }
  
  // ������ʾ
  if(get_sm_ss_state(to) == SM_FUSE_TIMER_ARMED && ev == EV_250MS) {
    
    return;
  }
  
  // �ȴ���������
  if(get_sm_ss_state(to) == SM_FUSE_TIMER_ARMED 
    && get_sm_ss_state(from) == SM_FUSE_TIMER_VERIFY 
    && ev == EV_KEY_MOD_PRESS) {
    
    return;
  } 
  
  // ��һ����
  if(get_sm_ss_state(to) == SM_FUSE_TIMER_VERIFY 
    && get_sm_ss_state(from) == SM_FUSE_TIMER_VERIFY 
    && ev == EV_KEY_MOD_PRESS) {
    
    return;
  }

}