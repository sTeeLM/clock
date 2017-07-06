#include "gyro.h"
#include "debug.h"
#include "serial_hub.h"
#include "i2c.h"
#include "task.h"

#define GYRO_I2C_ADDRESS  0x44 //0100 0100

static bit is_acc;
static bit is_rotate;
static bit is_drop;

void gyro_initialize (void)
{
  unsigned char val;
  
  CDBG("gyro_initialize\n");
  // Configuration Register 设置为全1，用于input
  I2C_Put(GYRO_I2C_ADDRESS, 0x3, 0xFF);
  // Polarity Inversion Register 设置为全0
  I2C_Put(GYRO_I2C_ADDRESS, 0x2, 0x0);
  // 读取一次端口寄存器消除中断
  I2C_Get(GYRO_I2C_ADDRESS, 0x0, &val);
  CDBG("gyro port reg is %bx\n", val);
}

void scan_gyro(void)
{
  unsigned char val;
  CDBG("scan_gyro\n");
  // 读取一次端口寄存器消除中断
  I2C_Get(GYRO_I2C_ADDRESS, 0x0, &val);
  CDBG("gyro port reg is %bx\n", val);
  
  if ((val & 0x1) == 0) {
    CDBG("EV_ROTATE_GYRO\n");
    set_task(EV_ROTATE_GYRO);
  } else if((val & 0x2) == 0){
    CDBG("EV_DROP_GYRO\n");
    set_task(EV_DROP_GYRO);
  } else if((val & 0x4) == 0) {
    CDBG("EV_ACC_GYRO\n");
    set_task(EV_ACC_GYRO);
  }
}

void gyro_enable(bit enable)
{
	CDBG("gyro_enable %bd\n", enable);
	serial_set_ctl_bit(SERIAL_BIT_GYRO_EN, enable);
	serial_ctl_out();
}
