#ifndef __CLOCK_TRIPWIRE_H__
#define __CLOCK_TRIPWIRE_H__
#include "task.h"
void tripwire_initialize (void);
void scan_tripwire(unsigned int status);
void tripwire_proc(enum task_events ev);

void tripwire_enable(bit enable);
void tripwire_set_broke(bit broke);

#endif