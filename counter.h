#ifndef __CLOCK_COUNT_DOWN_H__
#define __CLOCK_COUNT_DOWN_H__

#include "task.h"

void counter_initialize(void);
void counter_enter_powersave(void);
void counter_leave_powersave(void);
void counter_proc(enum task_events ev);

#endif