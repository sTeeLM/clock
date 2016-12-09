#include "task.h"
#include "sm_display.h"

void sm_display(unsigned char from, unsigned char to, enum task_events ev)
{
  // 从别的状态切过来,防止误操作
  if(get_sm_ss_state(from) == SM_DISPLAY_INIT) {
    
  }
  
}