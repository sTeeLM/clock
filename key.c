#include <STC89C5xRC.H>

#include "key.h"
#include "task.h"
#include "clock.h"
#include "debug.h"
#include "misc.h"
#include "sm.h"
#include "cext.h"
#include "power.h"
#include "beeper.h"

#define KEY_PRESS_DELAY 200 // 防止抖动，检测延迟时间，200 us
#define KEY_LPRESS_DELAY 3 // 长按时间，5s
#define KEY_2_KEY_LPRESS_DELAY 2 // 同时按下长按时间，2s

static unsigned char last_mod_tmr_count; 
static unsigned char last_set_tmr_count;

sbit SET_KEY = P2 ^ 7;
sbit MOD_KEY = P2 ^ 6;

static bit mod_press;
static bit set_press;

static void int0_ISR (void) interrupt 0 using 1
{
  IE0 = 0; // 清除中断标志位
  if(power_test_flag()) {
    power_clr_flag();
    set_task(EV_KEY_MOD_PRESS); // 为了能唤醒
  }
  beeper_stop_music();// 按键停止音乐
	
}

void scan_key_proc(enum task_events ev)
{
  if(!SET_KEY && !set_press) {
    delay_5us(100);
    if(!SET_KEY) {
      set_task(EV_KEY_SET_DOWN);      
      set_press = 1;
      last_set_tmr_count = clock_get_sec_256();     
    }
  } else if(!SET_KEY && set_press){
    if(time_diff_now(last_set_tmr_count) >= KEY_LPRESS_DELAY) {
      set_task(EV_KEY_SET_LPRESS);
    }
  } else if(SET_KEY && set_press) {
    set_task(EV_KEY_SET_UP);
    set_press = 0;
    if(time_diff_now(last_set_tmr_count) < KEY_LPRESS_DELAY) {
      set_task(EV_KEY_SET_PRESS);
      if(mod_press) {
        set_task(EV_KEY_MOD_SET_PRESS);    
      }         
    }    
  }
	
  if(!MOD_KEY && !mod_press) {
    delay_5us(100);
    if(!MOD_KEY) {
      set_task(EV_KEY_MOD_DOWN);     
      mod_press = 1;
      last_mod_tmr_count = clock_get_sec_256();       
    }
  } else if(!MOD_KEY && mod_press){
    if(time_diff_now(last_mod_tmr_count) >= KEY_LPRESS_DELAY) {
      set_task(EV_KEY_MOD_LPRESS);
    }
  } else if(MOD_KEY && mod_press) {
    set_task(EV_KEY_MOD_UP);
    if(time_diff_now(last_mod_tmr_count) < KEY_LPRESS_DELAY) {
      set_task(EV_KEY_MOD_PRESS);
      if(set_press) {
        set_task(EV_KEY_MOD_SET_PRESS);    
      }        
    }
    mod_press = 0;
  }
  
  if(!SET_KEY && set_press 
    && !MOD_KEY && mod_press) {
    if(time_diff_now(last_mod_tmr_count) >= KEY_2_KEY_LPRESS_DELAY
      && time_diff_now(last_set_tmr_count) >= KEY_2_KEY_LPRESS_DELAY) {
      set_task(EV_KEY_MOD_SET_LPRESS);
    }
  }
}

void mod_proc(enum task_events ev)
{
  CDBG("mod_proc\n");

  switch (ev) {
    case EV_KEY_MOD_DOWN:
      CDBG("mod_proc EV_KEY_MOD_DOWN\n");
      beeper_beep();
      break;
    case EV_KEY_MOD_UP:
      CDBG("mod_proc EV_KEY_MOD_UP\n");
      break; 
    case EV_KEY_MOD_PRESS:
      CDBG("mod_proc EV_KEY_MOD_PRESS\n");
      break;
    case EV_KEY_MOD_LPRESS:
      CDBG("mod_proc EV_KEY_MOD_LPRESS\n");
      break;     
  }
  
  run_state_machine(ev);

}

void set_proc(enum task_events ev)
{
  CDBG("set_proc %bd\n", ev);
  
  switch (ev) {
    case EV_KEY_SET_DOWN:
      CDBG("set_proc EV_KEY_SET_DOWN\n");
      beeper_beep();
      break;
    case EV_KEY_SET_UP:
      CDBG("set_proc EV_KEY_SET_UP\n");
      break; 
    case EV_KEY_SET_PRESS:
      CDBG("set_proc EV_KEY_SET_PRESS\n");
      break;
    case EV_KEY_SET_LPRESS:
      CDBG("set_proc EV_KEY_SET_LPRESS\n");
      break;     
  }
  
  run_state_machine(ev);
}

void mod_set_proc(enum task_events ev)
{
  CDBG("mod_set_proc %bd\n", ev);
  
  switch (ev) {
    case EV_KEY_MOD_SET_PRESS:
      CDBG("mod_set_proc EV_KEY_MOD_SET_PRESS\n");
      break;
    case EV_KEY_MOD_SET_LPRESS:
      CDBG("mod_set_proc EV_KEY_MOD_SET_LPRESS\n");
      break;    
  }

  run_state_machine(ev);
}

void key_initialize (void)
{
  CDBG("key_initialize\n");

  last_mod_tmr_count = 0;
  last_set_tmr_count = 0;
   
  mod_press = 0; 
  set_press = 0;
  
  IT0 = 1; // 设置为边沿触发
  EX0 = 1; // 开中断
}

void key_enter_powersave(void)
{
  CDBG("key_enter_powersave\n");
}

void key_leave_powersave(void)
{
  CDBG("key_leave_powersave\n");
}

