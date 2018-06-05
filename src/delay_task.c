#include "delay_task.h"
#include "debug.h"
#include "debug.h"

static DELAY_TASK_PROC task;

static struct delay_task delay_tasks[DELAU_TASK_CNT];

void delay_task_initialize(void)
{
  CDBG("delay_task_initialize\n");
}

void delay_task_reg(unsigned char index, DELAY_TASK_PROC task, unsigned char delay_sec)
{
  CDBG("delay_task_reg %bu, delay_sec = %bu\n", index, delay_sec);
  if(index > DELAU_TASK_CNT) {
    return;
  }
  
  delay_tasks[index].task = task;
  delay_tasks[index].delay_sec = delay_sec;
}

// call every sec
void delay_task_call(void) 
{
  unsigned char i;
  
  for(i = 0 ; i < DELAU_TASK_CNT ; i++) {
    if(delay_tasks[i].task != NULL && --delay_tasks[i].delay_sec == 0) {
      CDBG("delay_task_call: index = %bu\n", i);
      delay_tasks[i].task(); 
      delay_tasks[i].task = NULL;
    }
  }
}
