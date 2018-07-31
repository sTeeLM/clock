#ifndef __CLOCK_THERMO_H__
#define __CLOCK_THERMO_H__

#include "task.h"

#define THERMO_THRESHOLD_INVALID (128)
#define THERMO_THRESHOLD_MAX (85)
#define THERMO_THRESHOLD_MIN (-55)

void thermo_initialize (void);
void scan_thermo(unsigned int status);
void thermo_proc(enum task_events ev);

void thermo_hi_enable(bit enable); 
void thermo_lo_enable(bit enable); 

bit thermo_hi_threshold_reach_bottom(void);
bit thermo_hi_threshold_reach_top(void);
bit thermo_lo_threshold_reach_bottom(void);
bit thermo_lo_threshold_reach_top(void);

char thermo_hi_threshold_get(void); 
char thermo_lo_threshold_get(void);

void thermo_hi_threshold_dec(void); 
void thermo_hi_threshold_inc(void);

void thermo_lo_threshold_dec(void);
void thermo_lo_threshold_inc(void);

void thermo_hi_threshold_set(char val);
void thermo_lo_threshold_set(char val);

void thermo_hi_threshold_reset(void);
void thermo_lo_threshold_reset(void);

unsigned char thermo_threshold_inc(unsigned char thres);

char thermo_get_current(void);

void thermo_enter_powersave(void);
void thermo_leave_powersave(void);

#endif
