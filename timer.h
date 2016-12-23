#ifndef __CLOCK_TIMER_H__
#define __CLOCK_TIMER_H__

#include "task.h"

void timer_initialize(void);
void timer_enter_powersave(void);
void timer_leave_powersave(void);
void timer_proc(enum task_events ev);

#endif