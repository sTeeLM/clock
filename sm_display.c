#include "task.h"
#include "sm_display.h"

void sm_display(unsigned char from, unsigned char to, enum task_events ev)
{
  // �ӱ��״̬�й���,��ֹ�����
  if(get_sm_ss_state(from) == SM_DISPLAY_INIT) {
    
  }
  
}