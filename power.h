#ifndef __CLOCK_POWER_H__
#define __CLOCK_POWER_H__

#include "task.h"

void power_initialize(void);
void power_proc(enum task_events ev);

#endif