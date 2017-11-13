#ifndef __CLOCK_DELAY_TASK_H__
#define __CLOCK_DELAY_TASK_H__

typedef void (code *DELAY_TASK_PROC)(void);

void delay_task_reg(DELAY_TASK_PROC task, unsigned int delay_sec);

#endif
