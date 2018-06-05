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

unsigned char power_get_powersave_to_s(void);
enum powersave_time power_get_powersave_to(void);
void power_inc_powersave_to();
bit power_test_powersave_to(void);
void power_reset_powersave_to(void);

void power_proc(enum task_events ev);

void power_set_flag(void);
void power_clr_flag(void);
bit power_test_flag(void);

void power_5v_enable(bit enable);
bit power_5v_get_enable(void);

unsigned char power_get_volume(void);

#endif
