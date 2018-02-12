#ifndef __CLOCK_GYRO_H__
#define __CLOCK_GYRO_H__
#include "task.h"
void gyro_initialize (void);
void scan_gyro(void);

void gyro_enable(bit enable);

void gyro_enter_powersave(void);
void gyro_leave_powersave(void);

#endif