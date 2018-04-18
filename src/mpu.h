#ifndef __CLOCK_MPU_H__
#define __CLOCK_MPU_H__

#include "task.h"
void mpu_initialize (void);
void mpu_enter_powersave(void);
void mpu_leave_powersave(void);
void scan_mpu(void);
void mpu_enable(bit enable);
void mpu_read_acc(int * x, int * y, int * z);
unsigned char mpu_read_int_status(void);
#endif