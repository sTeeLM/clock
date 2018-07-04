#ifndef __CLOCK_REMOTE_H__
#define __CLOCK_REMOTE_H__

#include "task.h"

void remote_initialize (void);
void remote_enter_powersave(void);
void remote_leave_powersave(void);

void scan_remote(unsigned int status);

void remote_proc(enum task_events ev);

// 遥控器总电源， R_MOD/R_KEY可用
void remote_enable(bit enable); 
bit  remote_get_enable(void);

// 如果打开，是否还额外允许DISARM/DETONATE按钮
void remote_fuse_enable(bit enable); 

#endif