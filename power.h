#ifndef __CLOCK_POWER_H__
#define __CLOCK_POWER_H__

#include "task.h"

enum powersave_time{
  POWERSAVE_OFF  = 0,
  POWERSAVE_15S  = 1,
  POWERSAVE_30S  = 2,
  POWERSAVE_CNT
};

void power_initialize(void);
void power_enter_powersave(void);
void power_leave_powersave(void);

unsigned char get_powersave_to_s(void);
enum powersave_time get_powersave_to(void);
void set_powersave_to(enum powersave_time to);
bit test_powersave_to(void);
void reset_powersave_to(void);

void power_proc(enum task_events ev);

#endif