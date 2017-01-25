#ifndef __CLOCK_HG_H__
#define __CLOCK_HG_H__
#include "task.h"
void hg_initialize (void);
void scan_hg(void);
void hg_proc(enum task_events ev);
#endif