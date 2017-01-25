#ifndef __CLOCK_TRIPWIRE_H__
#define __CLOCK_TRIPWIRE_H__
#include "task.h"
void tripwire_initialize (void);
void scan_tripwire(void);
void tripwire_proc(enum task_events ev);
#endif