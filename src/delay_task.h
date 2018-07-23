#ifndef __CLOCK_DELAY_TASK_H__
#define __CLOCK_DELAY_TASK_H__

typedef void (code *DELAY_TASK_PROC)(void);

enum delay_task_type {
  DELAY_TASK_HG = 0,
  DELAY_TASK_POWER,
  DELAY_TASK_CNT
};

struct delay_task {
  unsigned int delay_sec;
  DELAY_TASK_PROC task;
};

void delay_task_initialize(void);
void delay_task_reg(unsigned char index, DELAY_TASK_PROC task, unsigned int delay_sec);
void delay_task_call(void);
#endif
