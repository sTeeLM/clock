#ifndef __CLOCK_MPU_H__
#define __CLOCK_MPU_H__

#include "task.h"

#define MPU_THRESHOLD_INVALID 0xFF
#define MPU_THRESHOLD_MAX (99)
#define MPU_THRESHOLD_MIN (1)

void mpu_initialize (void);
void mpu_enter_powersave(void);
void mpu_leave_powersave(void);
void scan_mpu(void);
void mpu_enable(bit enable);
unsigned char mpu_threshold_inc(unsigned char val);
unsigned char mpu_threshold_get(void);
void mpu_threshold_set(unsigned char thres);
void mpu_threshold_reset(void);
unsigned char mpu_read_int_status(void);

#endif
