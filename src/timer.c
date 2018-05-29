#include "string.h"
#include "timer.h"
#include "debug.h"
#include "sm.h"
#include "mod_common.h"
#include "led.h"

static struct timer_struct tmr[TIMER_SLOT_CNT]; // slot0 当前timer，1/2是两个瞬时值

static unsigned char tmr_mode;
static bit tmr_display;
static bit tmr_countdown_stop;
static bit tmr_disp_mmssms;
static bit tmr_start;


#pragma NOAREGS
void timer_inc_ms39(void)
{
  unsigned char ms10;
  
  if(!tmr_start)
    return;
  
  if(tmr_mode == TIMER_MODE_INC) {
    tmr[0].ms39 ++;
    if(tmr[0].ms39 == 0 ) {
      tmr[0].sec = ( ++tmr[0].sec) % 60;
      if(tmr[0].sec == 0) {
        tmr[0].min = (++ tmr[0].min) % 60;
        if(tmr[0].min == 0) {
          tmr[0].hour = (++ tmr[0].hour) % 24;
        }
      }
    }
  } else {
    if(!tmr_countdown_stop) {
      if(tmr[0].ms39 != 0
        || tmr[0].sec != 0
        || tmr[0].min != 0
        || tmr[0].hour != 0) {
        tmr[0].ms39 --;
        if(tmr[0].ms39 == 255 ) {
          tmr[0].sec --;
          if(tmr[0].sec == 255){
            tmr[0].sec = 59;
            tmr[0].min --;
            if(tmr[0].min == 255){ 
              tmr[0].min = 59;
              if(tmr[0].hour > 0) {
                tmr[0].hour --;
              }
            }
          }
        }
      } else {
        tmr_countdown_stop = 1;
        set_task(EV_COUNTER);
      }
    }
  }
  if(tmr_display) {
    ms10 = (unsigned char)(((float)(tmr[0].ms39) * 3.9) / 10);
    if(tmr_disp_mmssms) {    
      led_data[5] = led_code[(tmr[0].min / 10) + 4]; 
      led_data[4] = led_code[(tmr[0].min % 10) + 4];      
      led_data[3] = led_code[(tmr[0].sec / 10) + 4]; 
      led_data[2] = led_code[(tmr[0].sec % 10) + 4];    
      led_data[1] = led_code[(ms10 / 10) + 4]; 
      led_data[0] = led_code[(ms10 % 10) + 4]; 
    } else {
      led_data[5] = led_code[(tmr[0].hour / 10) + 4]; 
      led_data[4] = led_code[(tmr[0].hour % 10) + 4];      
      led_data[3] = led_code[(tmr[0].min / 10) + 4]; 
      led_data[2] = led_code[(tmr[0].min % 10) + 4];    
      led_data[1] = led_code[(tmr[0].sec / 10) + 4]; 
      led_data[0] = led_code[(tmr[0].sec % 10) + 4]; 
    }
    led_data[0] |= 0x80;
    led_data[5] |= 0x80;  
  }
}
#pragma AREGS 

void timer_initialize(void)
{
  CDBG("clock_initialize\n");

  tmr_display = 0;
  tmr_countdown_stop = 1;
  tmr_disp_mmssms = 0;
  tmr_start = 0;
}

void timer_enter_powersave(void)
{
  CDBG("timer_enter_powersave\n");
  timer_clr(); // 一旦进入节电模式，自动清0，节电模式无法用！
}

void timer_leave_powersave(void)
{
  CDBG("timer_leave_powersave\n");
}

void timer_set_led_autorefresh(bit enable, enum timer_display_mode mode)
{
  tmr_display = enable;
  if(mode == TIMER_DISP_MODE_HHMMSS) {
    tmr_disp_mmssms = 0;
  } else {
    tmr_disp_mmssms = 1;
  }
}

void timer_set_mode(enum timer_mode mode)
{
  tmr_mode = mode;
}

void timer_start(void)
{
  CDBG("timer_start mode is %bd\n", tmr_mode);
  tmr_countdown_stop = 0;
  tmr_start = 1;
}

void timer_save(unsigned char slot)
{
  if(slot > 0 && slot < TIMER_SLOT_CNT) {
    memcpy(&tmr[slot], &tmr[0], sizeof(tmr[0]));
  }
}

unsigned char timer_get_hour(unsigned char slot)
{
  if(slot >= 0 && slot < TIMER_SLOT_CNT) {
    return tmr[slot].hour;
  }
  return 0;
}
void timer_inc_hour(unsigned char slot)
{
  if(slot >= 0 && slot < TIMER_SLOT_CNT) {
    tmr[slot].hour = (++tmr[slot].hour) % 100;
  }
}

unsigned char timer_get_min(unsigned char slot)
{
  if(slot >= 0 && slot < TIMER_SLOT_CNT) {
    return tmr[slot].min;
  }
  return 0;
}
void timer_inc_min(unsigned char slot)
{
  if(slot >= 0 && slot < TIMER_SLOT_CNT) {
    tmr[slot].min = (++tmr[slot].min) % 60;
  }
}


unsigned char timer_get_sec(unsigned char slot)
{
  if(slot >= 0 && slot < TIMER_SLOT_CNT) {
    return tmr[slot].sec;
  }
  return 0;
}
void timer_inc_sec(unsigned char slot)
{
  if(slot >= 0 && slot < TIMER_SLOT_CNT) {
    tmr[slot].sec = (++ tmr[slot].sec) % 60;
  }
}

unsigned char timer_get_ms10(unsigned char slot)
{
  
  if(slot >= 0 && slot < TIMER_SLOT_CNT) {
    return (unsigned char)(((float)(tmr[slot].ms39) * 3.9) / 10);
  }
  return 0;
}

void timer_stop(void)
{
  tmr_start = 0;
}

void timer_clr(void)
{
  memset(tmr, 0, sizeof(tmr));
  
  tmr_display = 0;
  tmr_countdown_stop = 1;
  tmr_disp_mmssms = 0;
  tmr_start = 0;
}

