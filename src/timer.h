#ifndef __CLOCK_TIMER_H__
#define __CLOCK_TIMER_H__

#include "task.h"

void timer_initialize(void);
void timer_enter_powersave(void);
void timer_leave_powersave(void);
void timer_inc_ms39(void);
enum timer_mode {
  TIMER_MODE_INC = 0,
  TIMER_MODE_DEC = 1,  
};

enum timer_display_mode
{
  TIMER_DISP_MODE_HHMMSS = 0,
  TIMER_DISP_MODE_MMSSMM = 1,
};

struct timer_struct {
  unsigned char hour;
  unsigned char min;
  unsigned char sec;
  unsigned char ms39;
};

#define TIMER_SLOT_CNT 5

void timer_set_led_autorefresh(bit enable, enum timer_display_mode mode);
void timer_set_mode(enum timer_mode mode);
void timer_start(void);
void timer_save(unsigned char slot);
unsigned char timer_get_hour(unsigned char slot);
void timer_inc_hour(unsigned char slot);
unsigned char timer_get_min(unsigned char slot);
void timer_inc_min(unsigned char slot);
unsigned char timer_get_sec(unsigned char slot);
void timer_inc_sec(unsigned char slot);
unsigned char timer_get_ms10(unsigned char slot);
void timer_stop(void);
void timer_clr(void);

#endif