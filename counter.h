#ifndef __CLOCK_COUNT_DOWN_H__
#define __CLOCK_COUNT_DOWN_H__

#include "task.h"

void counter_initialize(void);
void counter_proc(enum task_events ev);

#endif