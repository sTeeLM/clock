#include "sm_fuse_mode.h"
#include "debug.h"
#include "mod_common.h"
#include "led.h"

#define FUSE_MODE_DISPLAY_TIMER   0
#define FUSE_MODE_DISPLAY_GERNADE 1


static void display_mode(unsigned char mode)
{
  led_clear();
  led_set_code(5, 'P');
	led_set_code(4, 'L');
	led_set_code(3, '-'); 
  switch(mode) {
    case FUSE_MODE_DISPLAY_TIMER:
      led_set_code(2, '0');
      break;
    case FUSE_MODE_DISPLAY_GERNADE:
      led_set_code(2, '1');
      break;
  }
}


void sm_fuse_mode(unsigned char from, unsigned char to, enum task_events ev)
{
  CDBG("sm_fuse_mode %bd %bd %bd\n", from, to, ev);

  // ��mod1����ģʽѡ��
  if(get_sm_ss_state(to) == SM_FUSE_MODE_INIT && ev == EV_KEY_MOD_LPRESS) {
    display_logo(DISPLAY_LOGO_TYPE_FUSE, 2);
    return;
  }
  
  // �ӱ��״̬�л���������ֹ�����
  if(get_sm_ss_state(from) == SM_FUSE_MODE_INIT
    && get_sm_ss_state(to) == SM_FUSE_MODE_TIMER) {
    display_mode(FUSE_MODE_DISPLAY_TIMER);
    return;
  }
    
  // mod0 �л�����ģʽ
  if(get_sm_ss_state(to) == SM_FUSE_MODE_TIMER && (ev == EV_KEY_MOD_PRESS || ev == EV_1S)) {
    display_mode(FUSE_MODE_DISPLAY_TIMER);
    return;
  }
  
  // set0 ѡ��
  if(get_sm_ss_state(to) == SM_FUSE_MODE_TIMER && ev == EV_KEY_SET_PRESS) {
    set_task(EV_FUSE_SEL0);
    return;
  }
  
  // mod0 �л�����ģʽ
  if(get_sm_ss_state(to) == SM_FUSE_MODE_GRENADE && (ev == EV_KEY_MOD_PRESS || ev == EV_1S)) {
    display_mode(FUSE_MODE_DISPLAY_GERNADE);
    return;
  }
  
  // set0 ѡ��
  if(get_sm_ss_state(to) == SM_FUSE_MODE_GRENADE && ev == EV_KEY_SET_PRESS) {
    set_task(EV_FUSE_SEL0);
    return;
  }
}