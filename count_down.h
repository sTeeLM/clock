#ifndef __CLOCK_COUNT_DOWN_H__
#define __CLOCK_COUNT_DOWN_H__

#include "task.h"

void count_down_initialize(void);
void count_down_proc(enum task_events ev);

#endif