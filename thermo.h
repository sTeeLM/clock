#ifndef __CLOCK_THERMO_H__
#define __CLOCK_THERMO_H__

#include "task.h"

void thermo_initialize (void);
void scan_thermo(void);
void thermo_proc(enum task_events ev);
#endif