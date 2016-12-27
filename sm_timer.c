#include "sm_timer.h"
#include "mod_common.h"
#include "led.h"
#include "timer.h"
#include "debug.h"
#include "beeper.h"

static unsigned char slot;

static void show_slot_title(unsigned char slot)
{
  led_clear();
  led_set_code(5, 'S');
  led_set_code(4, 'L');
  led_set_code(3, 'O');
  led_set_code(1, (slot / 10) + 0x30);  
  led_set_code(0, (slot % 10) + 0x30);  
}

static void display_slot(unsigned char slot)
{
  led_clear();
  // 两个“:”号
  led_set_dp(1);
  led_set_dp(2);
  led_set_dp(3);
  led_set_dp(4);
  led_set_code(5, (timer_get_min(slot) % 10) + 0x30);
  led_set_code(4, (timer_get_min(slot) % 10) + 0x30);
  led_set_code(3, (timer_get_sec(slot) / 10)+ 0x30);
  led_set_code(2, (timer_get_sec(slot) % 10) + 0x30);
  led_set_code(1, (timer_get_ms10(slot) / 10) + 0x30);
  led_set_code(0, (timer_get_ms10(slot) % 10) + 0x30);
}

void sm_timer(unsigned char from, unsigned char to, enum task_events ev)
{
  CDBG("sm_timer %bd %bd %bd\n", from, to, ev);
  
  // 按set1跑表大模式
  if(get_sm_ss_state(to) == SM_TIMER_INIT && ev == EV_KEY_SET_LPRESS) {
    display_logo(4);
    return;
  }
  
  // 切换到跑表大模式
  if(get_sm_ss_state(from) == SM_TIMER_INIT 
    && get_sm_ss_state(to) == SM_TIMER_CLEAR
    && ev == EV_KEY_SET_UP) {
    display_slot(0);
    timer_set_mode(TIMER_MODE_INC);
    slot = 1;
    return;
  }
  
  // mod0跑表开始跑
  if(get_sm_ss_state(to) == SM_TIMER_RUNNING && ev == EV_KEY_MOD_DOWN) {
    timer_set_led_autorefresh(1, TIMER_DISP_MODE_MMSSMM);
    timer_start();
    slot = 1;
    beeper_beep();
    return;
  }
  
  // set0计次
  if(get_sm_ss_state(to) == SM_TIMER_RUNNING && ev == EV_KEY_SET_DOWN) {
    if(slot < TIMER_SLOT_CNT) {
      timer_set_led_autorefresh(0, TIMER_DISP_MODE_MMSSMM);
      show_slot_title(slot);
    }
    return;
  }
  
  if(get_sm_ss_state(to) == SM_TIMER_RUNNING && ev == EV_KEY_SET_UP) {
    if(slot < TIMER_SLOT_CNT) {
      timer_set_led_autorefresh(1, TIMER_DISP_MODE_MMSSMM);
      timer_save(slot);
      slot ++;
      beeper_beep();
    }
    return;
  }  
  
  // mod0跑表停止
  if(get_sm_ss_state(to) == SM_TIMER_STOP && ev == EV_KEY_MOD_DOWN) {
    timer_stop();
    timer_set_led_autorefresh(0, TIMER_DISP_MODE_MMSSMM);
    display_slot(0);
    slot = 1;
    beeper_beep();
    return;
  }
  
  // mod0跑表清0
  if(get_sm_ss_state(to) == SM_TIMER_CLEAR && ev == EV_KEY_MOD_DOWN) {
    timer_clr();
    display_slot(0);
    beeper_beep();
    return;
  }
  
  // set0逐次显示计次
  if(get_sm_ss_state(to) == SM_TIMER_STOP && ev == EV_KEY_SET_DOWN) {
    show_slot_title(slot);
    return;
  }  
  
  if(get_sm_ss_state(to) == SM_TIMER_STOP && ev == EV_KEY_SET_UP) {
    display_slot(slot);
    slot = (++ slot) % TIMER_SLOT_CNT;
    beeper_beep();
    return;
  }
  
}
