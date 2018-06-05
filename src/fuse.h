#ifndef __CLOCK_FUSE_H__
#define __CLOCK_FUSE_H__
#include "task.h"
void fuse_initialize (void);
void scan_fuse(unsigned int status);
void fuse_proc(enum task_events ev);

void fuse_set_fuse_broke(unsigned char index, bit enable);
void fuse_set_tripwire_broke(bit broke);

// 如果fuse_enable(0), fuse_broke,tripwire_broke, trigger全部disable
void fuse_enable(bit enable);

void fuse_enter_powersave(void);
void fuse_leave_powersave(void);

void fuse_trigger(bit enable);

#endif
