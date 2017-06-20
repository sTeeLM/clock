#ifndef __CLOCK_FUSE_H__
#define __CLOCK_FUSE_H__
#include "task.h"
void fuse_initialize (void);
void scan_fuse(void);
void fuse_proc(enum task_events ev);


void fuse_set_fuse_short(unsigned char index, bit enable);
void fuse_set_fuse_broke(unsigned char index, bit enable);
void fuse_enable(bit enable);
#endif