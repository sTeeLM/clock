#ifndef __CLOCK_INT_HUB_H__
#define __CLOCK_INT_HUB_H__

#include "task.h"

void int_hub_initialize (void);
void scan_int_hub_proc (enum task_events ev);

enum int_hub_state {
  INT_HUB_FUSE0_BROKE=0,
  INT_HUB_FUSE1_BROKE,
  INT_HUB_HG0_HIT,
  INT_HUB_HG1_HIT,
  INT_HUB_HG2_HIT, 
  INT_HUB_HG3_HIT,
  INT_HUB_TRIPWIRE_HIT,
  INT_HUB_THERMO_HI_HIT,
  INT_HUB_THERMO_LO_HIT,
  INT_HUB_REMOTE_ARM, 
  INT_HUB_REMOTE_DISARM,    
  INT_HUB_REMOTE_DETONATE, 
  INT_HUB_UNSUSED0,
  INT_HUB_UNSUSED1, 
  INT_HUB_UNSUSED2,
  INT_HUB_UNSUSED3,
};

bit int_hub_test_bit(unsigned char index, unsigned int status);
unsigned int int_hub_get_status(void);
void int_hub_dump_ext_status(unsigned int status);
void int_hub_dump(void);
#endif
