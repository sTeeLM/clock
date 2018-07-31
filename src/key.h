#ifndef __CLOCK_KEY_H__
#define __CLOCK_KEY_H__

#include "task.h"

void key_initialize (void);

void key_enter_powersave(void);
void key_leave_powersave(void);

void scan_key_proc (enum task_events ev);
void mod_proc (enum task_events ev);
void set_proc (enum task_events ev);
void mod_set_proc (enum task_events ev);

#endif
