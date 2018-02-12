#include "gyro.h"
#include "debug.h"
#include "serial_hub.h"
#include "i2c.h"
#include "task.h"
#include "power.h"

#define GYRO_I2C_ADDRESS  0x44 //0100 0100

static bit is_acc;
static bit is_rotate;
static bit is_drop;

static bit gyro_enabled;

static void gyro_power_on(void)
{
  unsigned char val;
  CDBG("gyro_power_on\n");
  serial_set_ctl_bit(SERIAL_BIT_GYRO_EN, 0);
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
  serial_set_ctl_bit(SERIAL_BIT_GYRO_EN, 1);
  serial_ctl_out();
}

void gyro_initialize (void)
{
  CDBG("gyro_initialize\n");
  gyro_enabled = 0;

}

void gyro_enter_powersave(void)
{
  CDBG("gyro_enter_powersave\n");
}

void gyro_leave_powersave(void)
{
  CDBG("gyro_leave_powersave\n");
}

void scan_gyro(void)
{
  unsigned char val;
  bit has_event = 0;
  
  CDBG("scan_gyro\n");
  
  // 读取一次端口寄存器消除中断
  I2C_Get(GYRO_I2C_ADDRESS, 0x0, &val);
  CDBG("gyro port reg is %bx\n", val);
  
  if ((val & 0x1) == 0) {
    CDBG("EV_ROTATE_GYRO\n");
    set_task(EV_ROTATE_GYRO);
    has_event = 1;
  } else if((val & 0x2) == 0){
    CDBG("EV_DROP_GYRO\n");
    set_task(EV_DROP_GYRO);
    has_event = 1;
  } else if((val & 0x4) == 0) {
    CDBG("EV_ACC_GYRO\n");
    set_task(EV_ACC_GYRO);
    has_event = 1;
  }
  
  if(has_event && power_test_flag()) {
    power_clr_flag();
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
