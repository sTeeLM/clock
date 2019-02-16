#ifndef __CLOCK_DELAY_TASK_H__
#define __CLOCK_DELAY_TASK_H__

typedef void (code *DELAY_TASK_PROC)(void);

enum delay_task_type { // 在一般上下文调用
  DELAY_TASK_POWER = 0,
  DELAY_TASK_CNT
};

struct delay_task {
  unsigned int delay_sec;
  DELAY_TASK_PROC task;
};

void delay_task_initialize(void);
void delay_task_reg(enum delay_task_type index, DELAY_TASK_PROC task, unsigned int delay_sec);
void delay_task_call(void);
#endif
