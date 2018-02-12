#include <stdlib.h>
#include "cmd_task.h"
#include "shell.h"
#include "task.h"

char cmd_task(char arg1, char arg2)
{
  
  unsigned char tsk, val;
  
  if(arg1 == 0) { // list all task
    task_dump();
    return 0;
  } else if(arg2 != 0) {
    tsk = atoi(shell_buf + arg1);
    val = atoi(shell_buf + arg2);
    
    if(tsk > EV_COUNT)
      return 1;
    
    if(val) {
      set_task(tsk);
    } else {
      clr_task(tsk);
    }
    return 0;
  }
  
  return 1;
}