#ifndef __CLOCK_HG_H__
#define __CLOCK_HG_H__
#include "task.h"
void hg_initialize (void);
void scan_hg(unsigned int status);

void hg_enable(bit enable);
unsigned char hg_get_state(void);

void hg_enter_powersave(void);
void hg_leave_powersave(void);

#endif
