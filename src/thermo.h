#ifndef __CLOCK_THERMO_H__
#define __CLOCK_THERMO_H__

#include "task.h"

#define THERMO_THRESHOLED_INVALID 0xFF

void thermo_initialize (void);
void scan_thermo(void);
void thermo_proc(enum task_events ev);

void thermo_enable(bit enable);

void thermo_hi_enable(bit enable);
void thermo_lo_enable(bit enable);

bit thermo_hi_threshold_reach_bottom();
bit thermo_hi_threshold_reach_top();
bit thermo_lo_threshold_reach_bottom();
bit thermo_lo_threshold_reach_top();

char thermo_hi_threshold_get();
char thermo_lo_threshold_get();

void thermo_hi_threshold_dec();
void thermo_hi_threshold_inc();
void thermo_lo_threshold_dec();
void thermo_lo_threshold_inc();

void thermo_hi_threshold_reset();
void thermo_lo_threshold_reset();

void thermo_hi_threshold_set(char val);
void thermo_lo_threshold_set(char val);

unsigned char thermo_threshold_inc(unsigned char thres);

char thermo_get_current(void);

void thermo_enter_powersave(void);
void thermo_leave_powersave(void);

#endif