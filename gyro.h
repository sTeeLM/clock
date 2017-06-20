#ifndef __CLOCK_GYRO_H__
#define __CLOCK_GYRO_H__
#include "task.h"
void gyro_initialize (void);
void scan_gyro(void);

void gyro_enable(bit enable);
bit gyro_test_acc_event(void);
bit gyro_test_drop_event(void);
bit gyro_test_rotate_event(void);

#endif