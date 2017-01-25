#ifndef __CLOCK_INT_HUB_H__
#define __CLOCK_INT_HUB_H__

#include "task.h"

void int_hub_initialize (void);
void scan_int_hub_proc (enum task_events ev);

#endif