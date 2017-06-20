#include "gyro.h"
#include "debug.h"
#include "serial_hub.h"

void gyro_initialize (void)
{
  CDBG("gyro_initialize\n");
}

void scan_gyro(void)
{
  CDBG("scan_gyro\n");
}

void gyro_enable(bit enable)
{
	CDBG("gyro_enable %bd\n", enable);
	serial_set_ctl_bit(SERIAL_BIT_GYRO_EN, enable);
	serial_ctl_out();
}

bit gyro_test_acc_event(void)
{
	return 1;
}

bit gyro_test_drop_event(void)
{
	return 1;
}

bit gyro_test_rotate_event(void)
{
	return 1;
}