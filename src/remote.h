#ifndef __CLOCK_REMOTE_H__
#define __CLOCK_REMOTE_H__

#include "task.h"

void remote_initialize (void);
void remote_enter_powersave(void);
void remote_leave_powersave(void);

void scan_remote(unsigned int status);

void remote_proc(enum task_events ev);

void remote_enable(bit enable);

#endif