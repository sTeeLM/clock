#include <STC89C5xRC.H>

#include "key.h"
#include "task.h"
#include "clock.h"
#include "debug.h"
#include "misc.h"
#include "sm.h"
#include "cext.h"
#include "power.h"

#define KEY_PRESS_DELAY 200 // ��ֹ����������ӳ�ʱ�䣬200 us
#define KEY_LPRESS_DELAY 2 // ����ʱ�䣬2s
#define KEY_2_KEY_LPRESS_DELAY 1 // ͬʱ���³���ʱ�䣬1s

static unsigned char last_mod_tmr_count; 
static unsigned char last_set_tmr_count;

sbit SET_KEY = P2 ^ 7;
sbit MOD_KEY = P2 ^ 6;

static bit mod_press;
static bit set_press;

static bit key_isr_hit;

static void key_ISR (void) interrupt 0 using 1
{
  IE0 = 0; // ����жϱ�־λ
  if(powersave_enabled) {
    powersave_enabled = 0;
    set_task(EV_KEY_MOD_PRESS); // Ϊ���ܻ���
  }
}

void scan_key_proc(enum task_events ev)
{
  if(!MOD_KEY && !mod_press) {
    delay_ms(2);
    if(!MOD_KEY) {
      set_task(EV_KEY_MOD_DOWN);
//      set_task(EV_KEY_MOD_PRESS);      
      mod_press = 1;
      last_mod_tmr_count = clock_get_sec();
//      if(mod_press) {
//        set_task(EV_KEY_MOD_SET_PRESS);    
//      }       
    }
  } else if(!MOD_KEY && mod_press){
    if(time_diff(clock_get_sec(), last_mod_tmr_count) >= KEY_LPRESS_DELAY) {
      set_task(EV_KEY_MOD_LPRESS);
    }
  } else if(MOD_KEY && mod_press) {
    set_task(EV_KEY_MOD_UP);
    if(time_diff(clock_get_sec(), last_mod_tmr_count) < KEY_LPRESS_DELAY) {
      set_task(EV_KEY_MOD_PRESS);
      if(set_press) {
        set_task(EV_KEY_MOD_SET_PRESS);    
      }        
    }
    mod_press = 0;
  }

  if(!SET_KEY && !set_press) {
    delay_ms(2);
    if(!SET_KEY) {
      set_task(EV_KEY_SET_DOWN);
      //set_task(EV_KEY_SET_PRESS);      
      set_press = 1;
      last_set_tmr_count = clock_get_sec();
//      if(mod_press) {
//        set_task(EV_KEY_MOD_SET_PRESS);    
//      }      
    }
  } else if(!SET_KEY && set_press){
    if(time_diff(clock_get_sec(), last_set_tmr_count) >= KEY_LPRESS_DELAY) {
      set_task(EV_KEY_SET_LPRESS);
    }
  } else if(SET_KEY && set_press) {
    set_task(EV_KEY_SET_UP);
    set_press = 0;
    if(time_diff(clock_get_sec(), last_set_tmr_count) < KEY_LPRESS_DELAY) {
      set_task(EV_KEY_SET_PRESS);
      if(mod_press) {
        set_task(EV_KEY_MOD_SET_PRESS);    
      }         
    }    
  }
  
  if(!SET_KEY && set_press 
    && !MOD_KEY && mod_press) {
    if(time_diff(clock_get_sec(), last_mod_tmr_count) >= KEY_2_KEY_LPRESS_DELAY
      && time_diff(clock_get_sec(), last_set_tmr_count) >= KEY_2_KEY_LPRESS_DELAY) {
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
  
  IT0 = 1; // ����Ϊ���ش���
  EX0 = 1; // ���ж�

  last_mod_tmr_count = 0;
  last_set_tmr_count = 0;
   
  mod_press = 0; 
  set_press = 0;
  key_isr_hit = 0;
}

void key_enter_powersave(void)
{
  CDBG("key_enter_powersave\n");
}

void key_leave_powersave(void)
{
  CDBG("key_leave_powersave\n");
}

