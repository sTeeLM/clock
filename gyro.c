#include "gyro.h"
#include "debug.h"
#include "serial_hub.h"
#include "i2c.h"

#define GYRO_I2C_ADDRESS  0x90 //10010000

static unsigned int current_temp;

void gyro_initialize (void)
{
  CDBG("gyro_initialize\n");
}

void scan_gyro(void)
{
  CDBG("scan_gyro .... ");
	I2C_Init();
	if(I2C_Gets(GYRO_I2C_ADDRESS, 0xAA, 2, &current_temp)) {
		CDBG("failed %x\n", current_temp);
	} else {
		CDBG("success %x\n", current_temp);
	}
}

void gyro_enable(bit enable)
{
	CDBG("gyro_enable %bd\n", enable);
	serial_set_ctl_bit(SERIAL_BIT_GYRO_EN, enable);
	serial_ctl_out();
}

bit gyro_test_acc_event(void)
{
	CDBG("gyro_test_acc_event %x\n", current_temp);
	return current_temp == 0xE700;
}

bit gyro_test_drop_event(void)
{
	CDBG("gyro_test_drop_event %x\n", current_temp);
	return current_temp == 0x1900;
}

bit gyro_test_rotate_event(void)
{
	CDBG("gyro_test_rotate_event %x\n", current_temp);
	return current_temp == 0x0;
}