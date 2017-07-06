#ifndef __CLOCK_INT_HUB_H__
#define __CLOCK_INT_HUB_H__

#include "task.h"

void int_hub_initialize (void);
void scan_int_hub_proc (enum task_events ev);

enum int_hub_state {
  INT_HUB_FUSE0_TRIGGERED = 0,
  INT_HUB_FUSE1_TRIGGERED,  
  INT_HUB_FUSE0_SHORT,
  INT_HUB_FUSE0_BROKE,
  INT_HUB_FUSE1_SHORT, 
  INT_HUB_FUSE1_BROKE,
  INT_HUB_UNSUSED0, 
  INT_HUB_UNSUSED1,
  INT_HUB_HG0_HIT,
  INT_HUB_HG1_HIT,
  INT_HUB_HG2_HIT, 
  INT_HUB_HG3_HIT,
  INT_HUB_TRIPWIRE_HIT,
  INT_HUB_UNSUSED2, 
  INT_HUB_UNSUSED3,
  INT_HUB_UNSUSED4, 
};

bit int_hub_test_bit(unsigned char index, unsigned int status);

#endif