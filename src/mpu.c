#include "mpu.h"
#include "debug.h"
#include "serial_hub.h"
#include "i2c.h"
#include "task.h"
#include "power.h"
#include "cext.h"
#include "misc.h"


static bit mpu_enabled;

#ifdef __CLOCK_EMULATE__

#define MPU_I2C_ADDRESS  0x44 //0100 0100

static bit is_acc;
static bit is_rotate;
static bit is_drop;

static void mpu_power_on(void)
{
  unsigned char val;
  CDBG("mpu\n");
  serial_set_ctl_bit(SERIAL_BIT_MPU_EN, 0);
  serial_ctl_out();
  
  // Configuration Register 设置为全1，用于input
  I2C_Put(MPU_I2C_ADDRESS, 0x3, 0xFF);
  // Polarity Inversion Register 设置为全0
  I2C_Put(MPU_I2C_ADDRESS, 0x2, 0x0);
  // 读取一次端口寄存器消除中断
  I2C_Get(MPU_I2C_ADDRESS, 0x0, &val);
  CDBG("mpu port reg is %bx\n", val);
}

static void mpu_power_off(void)
{
  CDBG("mpu_power_off\n");
  serial_set_ctl_bit(SERIAL_BIT_MPU_EN, 1);
  serial_ctl_out();
}

void mpu_initialize (void)
{
  CDBG("mpu_initialize\n");
  mpu_enabled = 0;

}

void mpu_enter_powersave(void)
{
  CDBG("mpu_enter_powersave\n");
}

void mpu_leave_powersave(void)
{
  CDBG("mpu_leave_powersave\n");
}

void scan_mpu(void)
{
  unsigned char val;
  bit has_event = 0;
  
  CDBG("scan_mpu\n");
  
  // 读取一次端口寄存器消除中断
  I2C_Get(MPU_I2C_ADDRESS, 0x0, &val);
  CDBG("mpu port reg is %bx\n", val);
  
  if ((val & 0x1) == 0) {
    CDBG("EV_ROTATE_MPU\n");
    set_task(EV_ROTATE_MPU);
    has_event = 1;
  } else if((val & 0x2) == 0){
    CDBG("EV_DROP_MPU\n");
    set_task(EV_DROP_MPU);
    has_event = 1;
  } else if((val & 0x4) == 0) {
    CDBG("EV_ACC_MPU\n");
    set_task(EV_ACC_MPU);
    has_event = 1;
  }
  
  if(has_event && power_test_flag()) {
    power_clr_flag();
  }
}

void mpu_enable(bit enable)
{
  CDBG("mpu_enable %bd\n", enable ? 1 : 0 );
  if(enable && !mpu_enabled) {
    mpu_power_on();
  } else if(!enable && mpu_enabled){
    mpu_power_off();
  }
  
  mpu_enabled = enable;
}

void mpu_read_acc(unsigned int * x, unsigned int * y, unsigned int * z)
{
	
}

unsigned char mpu_read_int_status(void)
{
	
}

#else

#define MPU_I2C_ADDRESS  0xD2 //1101 0010

unsigned char  mpu_read_int_status(void)
{
	unsigned char val;
	
	I2C_Get(MPU_I2C_ADDRESS, 0x3A, &val);
	
	return val;
}

static void mpu_power_on(void)
{
  unsigned char val[2];
  
  CDBG("mpu_power_on\n");
  
  /* Device reset */
  val[0] = 0x80;
  I2C_Puts(MPU_I2C_ADDRESS, 0x6B, 1, val);
  delay_ms(100);
  
  /* Signal Path Reset */
  val[0] = 0x07;
  I2C_Puts(MPU_I2C_ADDRESS, 0x68, 1, val);
  delay_ms(100);
  
  /* Wake up chip. */
  val[0] = 0x00;
  I2C_Puts(MPU_I2C_ADDRESS, 0x6B, 1, val);
  delay_ms(100);
	
	/* PWR_MGMT_1 , disable temp */
  val[0] = 0x08;
  I2C_Puts(MPU_I2C_ADDRESS, 0x6B, 1, val);
	
	/* PWR_MGMT_2, close gyro,LP_WAKE_CTRL = 1.25HZ */
  val[0] = 0x07;
  I2C_Puts(MPU_I2C_ADDRESS, 0x6C, 1, val);
  
	/* Set sample rate */
  val[0] = 0x07;
  I2C_Puts(MPU_I2C_ADDRESS, 0x19, 1, val);
	
	/* Set config, DLPF_CFG = 21 HZ */
  val[0] = 0x04;
  I2C_Puts(MPU_I2C_ADDRESS, 0x1A, 1, val);
  
  /* Set the accel full-scale range to 2G, ACCEL_HPF to 0.63Hz */
  val[0] = 0x04;
  I2C_Puts(MPU_I2C_ADDRESS, 0x1C, 1, val);
	
	/* MOT_THR to 1 */
  val[0] = 0x01;
  I2C_Puts(MPU_I2C_ADDRESS, 0x1F, 1, val);
	
	/* MOT_DUR to 1 */
  val[0] = 0x01;
  I2C_Puts(MPU_I2C_ADDRESS, 0x20, 1, val);
	
	/* Set interrupts */
	// 11100000 = 0xE0
	// INT_LEVEL = 1 (active low)
	// INT_OPEN  = 1 (open drain)
	// LATCH_INT_EN = 1 (no latch)
	// INT_RD_CLEAR = 0 (cleared only by reading INT_STATUS)
	// FSYNC_INT_LEVEL = 0
	// FSYNC_INT_EN = 0
	// I2C_BYPASS_EN = 0
	// resetved = 0
	val[0] = 0xE0;
	I2C_Puts(MPU_I2C_ADDRESS, 0x37, 1, val);
	
	/* MOT_EN = 1 */
	val[0] = 0x40;
	I2C_Puts(MPU_I2C_ADDRESS, 0x38, 1, val);
	
	/* Enter Accelerometer Only Low Power Mode */
	// 00101000 = 0x28
	// (i) Set CYCLE bit to 1
  // (ii) Set SLEEP bit to 0
  // (iii) Set TEMP_DIS bit to 1
	// 01000111 = 0x47
	// LP_WAKE_CTRL = 01 (5HZ)
	// STBY_XA = STBY_YA = STBY_ZA = 0
	// STBY_XG = STBY_YG = STBY_ZG = 1
	val[0] = 0x28; 
	I2C_Puts(MPU_I2C_ADDRESS, 0x6B, 1, val);
	val[0] = 0x47;
	I2C_Puts(MPU_I2C_ADDRESS, 0x6C, 1, val);
	
	mpu_enabled = 1;
}

static void mpu_power_off(void)
{
  unsigned char val[2];
	CDBG("mpu_power_off\n");
	
	// into sleep
	// 01001000 = 0x48
	// Set SLEEP bit to 1
	// 01111111 = 0x7F
	// LP_WAKE_CTRL = 01 (5HZ)
	// STBY_XA = STBY_YA = STBY_ZA = 1
	// STBY_XG = STBY_YG = STBY_ZG = 1
	val[0] = 0x48;
	I2C_Puts(MPU_I2C_ADDRESS, 0x6B, 1, val);
	val[0] = 0x7F;
	I2C_Puts(MPU_I2C_ADDRESS, 0x6C, 1, val);
	
	// disable interrupt
	val[0] = 0x00;
	I2C_Puts(MPU_I2C_ADDRESS, 0x38, 1, val);
	delay_ms(100);
	
	// read int status to clr int
	I2C_Get(MPU_I2C_ADDRESS, 0x3A, val);
	
	mpu_enabled = 0;
}
void mpu_initialize (void)
{

  CDBG("mpu_initialize\n");
	mpu_power_on();
	mpu_power_off();
}

void mpu_enter_powersave(void)
{
  CDBG("mpu_enter_powersave\n");
}

void mpu_leave_powersave(void)
{
  CDBG("mpu_leave_powersave\n");
}

void scan_mpu(void) 
{
  CDBG("scan_mpu\n");
}

void mpu_enable(bit enable)
{
  CDBG("mpu_enable %bd\n", enable ? 1 : 0 );
	if(enable)
		mpu_power_on();
	else
		mpu_power_off();
}

void mpu_read_acc(int * x, int * y, int * z)
{
	unsigned char val;
	int tmp;
	
	if(!mpu_enabled)
		return;
	
	I2C_Get(MPU_I2C_ADDRESS, 0x3B, &val);
	tmp = val;
	I2C_Get(MPU_I2C_ADDRESS, 0x3C, &val);
	tmp = tmp << 8;
	tmp |= val;
	*x = tmp;
	
	I2C_Get(MPU_I2C_ADDRESS, 0x3D, &val);
	tmp = val;
	I2C_Get(MPU_I2C_ADDRESS, 0x3E, &val);
	tmp = tmp << 8;
	tmp |= val;
	*y = tmp;
	
	I2C_Get(MPU_I2C_ADDRESS, 0x3F, &val);
	tmp = val;
	I2C_Get(MPU_I2C_ADDRESS, 0x40, &val);
	tmp = tmp << 8;
	tmp |= val;
	*z = tmp;
}

#endif