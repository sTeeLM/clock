#ifndef __CLOCK_ALARM_H__
#define __CLOCK_ALARM_H__

#include "task.h"

void alarm_initialize (void);
void alarm_proc(enum task_events ev);

#endif