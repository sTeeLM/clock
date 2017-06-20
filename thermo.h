#ifndef __CLOCK_THERMO_H__
#define __CLOCK_THERMO_H__

#include "task.h"

void thermo_initialize (void);
void scan_thermo(void);
void thermo_proc(enum task_events ev);

void thermo_hi_enable(bit enable);
void thermo_lo_enable(bit enable);
void thermo_hi_set_hit(bit hit);
void thermo_lo_set_hit(bit hit);

#endif