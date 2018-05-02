#ifndef __CLOCK_MPU_H__
#define __CLOCK_MPU_H__

#include "task.h"

#define MPU_THRESHOLED_INVALID 0xFF

void mpu_initialize (void);
void mpu_enter_powersave(void);
void mpu_leave_powersave(void);
void scan_mpu(void);
void mpu_enable(bit enable);
void mpu_read_acc(int * x, int * y, int * z);
unsigned char mpu_read_int_status(void);
unsigned char mpu_threshold_inc(unsigned char val);
unsigned char mpu_threshold_get(void);
#endif