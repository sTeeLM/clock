#include "gyro.h"
#include "debug.h"
#include "serial_hub.h"
#include "i2c.h"
#include "task.h"

#define GYRO_I2C_ADDRESS  0x44 //0100 0100

static bit is_acc;
static bit is_rotate;
static bit is_drop;

static bit gyro_enabled;

static void gyro_power_on(void)
{
  unsigned char val;
  CDBG("gyro_power_on\n");
	serial_set_ctl_bit(SERIAL_BIT_GYRO_EN, 1);
	serial_ctl_out();
  
  // Configuration Register 设置为全1，用于input
  I2C_Put(GYRO_I2C_ADDRESS, 0x3, 0xFF);
  // Polarity Inversion Register 设置为全0
  I2C_Put(GYRO_I2C_ADDRESS, 0x2, 0x0);
  // 读取一次端口寄存器消除中断
  I2C_Get(GYRO_I2C_ADDRESS, 0x0, &val);
  CDBG("gyro port reg is %bx\n", val);
}

static void gyro_power_off(void)
{
  CDBG("gyro_power_off\n");
	serial_set_ctl_bit(SERIAL_BIT_GYRO_EN, 0);
	serial_ctl_out();
}

void gyro_initialize (void)
{
  CDBG("gyro_initialize\n");
  gyro_enabled = 0;

}

void scan_gyro(void)
{
  unsigned char val;
  CDBG("scan_gyro\n");
  
  if(!gyro_enabled) return;
  
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
	CDBG("gyro_enable %bd\n", enable ? 1 : 0 );
  if(enable && !gyro_enabled) {
    gyro_power_on();
  } else if(!enable && gyro_enabled){
    gyro_power_off();
  }
  
  gyro_enabled = enable;
}
