#include "sm_clock_mod_global_flag.h"
#include "mod_common.h"
#include "led.h"
#include "beeper.h"
#include "power.h"
#include "clock.h"
#include "alarm.h"
#include "debug.h"

static void display_global_flag(unsigned char what)
{
  bit baoshi,is_24;

  baoshi = alarm1_test_enable();

  is_24 = !clock_get_hour_12();
  
  led_clear();
  switch(what) {
    case IS_PS:
      led_set_code(5, 'P');
      led_set_code(4, 'S');
      if(power_get_powersave_to_s() == 0) {
        led_set_code(2, 'O');
        led_set_code(1, 'F');
        led_set_code(0, 'F');
      } else {
        led_set_code(1, (power_get_powersave_to_s() / 10) + 0x30);
        led_set_code(0, (power_get_powersave_to_s() % 10) + 0x30);
      }
      break;
    case IS_BS:
      led_set_code(5, 'B');
      led_set_code(4, 'S');
      if(baoshi) {
        led_set_code(2, LED_CODE_BLACK);
        led_set_code(1, 'O');
        led_set_code(0, 'N');
      } else {
        led_set_code(2, 'O');
        led_set_code(1, 'F');
        led_set_code(0, 'F');
      }
      break;
    case IS_MUSIC:
      led_set_code(5, 'S');
      led_set_code(4, 'O');
      led_set_code(3, 'U');
      led_set_code(2, 'N');
      led_set_code(1, 'D');    
      led_set_code(0, beeper_get_music_index() + 1 + 0x30);
      break;
    case IS_BEEP:
      led_set_code(5, 'B');
      led_set_code(4, 'E');
      led_set_code(3, 'P');
      if(beeper_get_beep_enable()) {
        led_set_code(2, LED_CODE_BLACK);
        led_set_code(1, 'O');
        led_set_code(0, 'N');        
      } else {
        led_set_code(2, 'O');
        led_set_code(1, 'F');
        led_set_code(0, 'F');
      }
      break;
    case IS_1224:
      led_set_code(5, 'D');
      led_set_code(4, 'S');
      led_set_code(3, 'P');    
      if(is_24) {
        led_set_code(1, '2');
        led_set_code(0, '4');
      } else {
        led_set_code(1, '1');
        led_set_code(0, '2');
      }
      break;
  }
}

static void inc_write(unsigned char what)
{
  switch(what) {
    case IS_PS:
      power_inc_powersave_to();
      break;
    case IS_BS:
      alarm1_set_enable(!alarm1_test_enable());
      alarm1_sync_to_rtc();
      break;
    case IS_MUSIC:
      beeper_inc_music_index();
      beeper_play_music();
      break;
    case IS_BEEP:
      beeper_set_beep_enable(!beeper_get_beep_enable());
      break;
    case IS_1224:
      clock_set_hour_12(!clock_get_hour_12());
      clock_sync_to_rtc(CLOCK_SYNC_TIME);
      alarm0_set_hour_12(!alarm0_get_hour_12());
      alarm0_sync_to_rtc();
      break;
  }
}


void sm_clock_mod_global_flag(unsigned char from, unsigned char to, enum task_events ev)
{
  CDBG("sm_clock_mod_global_flag %bd %bd %bd\n", from, to, ev);
  
  // ��mod1�����޸�ȫ�ֱ�־λģʽ
  if(get_sm_ss_state(to) == SM_CLOCK_MODIFY_GLOBAL_FLAG_INIT && ev == EV_KEY_MOD_LPRESS) {
    display_logo(DISPLAY_LOGO_TYPE_CLOCK, 3);
    return;
  }

  // �л����޸�ȫ�ֱ�־λģʽ
  if(get_sm_ss_state(from) == SM_CLOCK_MODIFY_GLOBAL_FLAG_INIT 
    && get_sm_ss_state(to) == SM_CLOCK_MODIFY_GLOBAL_FLAG_PS
    && ev == EV_KEY_MOD_UP) {
    display_global_flag(IS_PS);
    return;
  }

  // set0 ʡ��ģʽ��ʱʱ������
  if(get_sm_ss_state(to) == SM_CLOCK_MODIFY_GLOBAL_FLAG_PS && ev == EV_KEY_SET_PRESS) {
    inc_write(IS_PS);
    display_global_flag(IS_PS);
    return;
  }
  
  // mod0 �����������㱨ʱon/off״̬
  if(get_sm_ss_state(to) == SM_CLOCK_MODIFY_GLOBAL_FLAG_BS && ev == EV_KEY_MOD_PRESS) {
    display_global_flag(IS_BS);
    return;
  }
  
  // set0 ���㱨ʱon/off
  if(get_sm_ss_state(to) == SM_CLOCK_MODIFY_GLOBAL_FLAG_BS && ev == EV_KEY_SET_PRESS) {
    inc_write(IS_BS);
    display_global_flag(IS_BS);
    return;
  }  
  
  // mod0 ������������ѡ��
  if(get_sm_ss_state(to) == SM_CLOCK_MODIFY_GLOBAL_FLAG_ALARM_MUSIC && ev == EV_KEY_MOD_PRESS) {
    display_global_flag(IS_MUSIC);
    return;
  }

  // set0 ������������
  if(get_sm_ss_state(to) == SM_CLOCK_MODIFY_GLOBAL_FLAG_ALARM_MUSIC && ev == EV_KEY_SET_PRESS) {
    inc_write(IS_MUSIC);
    display_global_flag(IS_MUSIC);
    return;
  }
  
  // mod0 ���밴�����򿪹ر�
  if(get_sm_ss_state(to) == SM_CLOCK_MODIFY_GLOBAL_FLAG_BEEP && ev == EV_KEY_MOD_PRESS) {
    display_global_flag(IS_BEEP);
    return;
  } 

  // set0 �������򿪹ر�
  if(get_sm_ss_state(to) == SM_CLOCK_MODIFY_GLOBAL_FLAG_BEEP && ev == EV_KEY_SET_PRESS) {
    inc_write(IS_BEEP);
    display_global_flag(IS_BEEP);
    return;
  }  
  
  // mod0 ����1224Сʱ����״̬
  if(get_sm_ss_state(to) == SM_CLOCK_MODIFY_GLOBAL_FLAG_1224 && ev == EV_KEY_MOD_PRESS) {
    display_global_flag(IS_1224);
    return;
  }
  
  // set0 1224ģʽ�л�
  if(get_sm_ss_state(to) == SM_CLOCK_MODIFY_GLOBAL_FLAG_1224 && ev == EV_KEY_SET_PRESS) {
    inc_write(IS_1224);
    display_global_flag(IS_1224);
    return;
  }
  
  // mod0 ��������ʡ��ģʽ״̬
  if(get_sm_ss_state(to) == SM_CLOCK_MODIFY_GLOBAL_FLAG_PS && ev == EV_KEY_MOD_PRESS) {
    display_global_flag(IS_PS);
    return;
  }
}