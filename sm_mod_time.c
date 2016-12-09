#include "sm_mod_time.h"

void sm_mod_time(unsigned char from, unsigned char to, enum task_events ev)
{
  // 从别的状态切过来,防止误操作
  if(get_sm_ss_state(to) == SM_MODIFY_TIME_INIT) {
    
  }
  
  
}