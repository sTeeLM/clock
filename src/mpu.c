#include "mpu.h"
#include "debug.h"
#include "serial_hub.h"
#include "i2c.h"
#include "task.h"
#include "power.h"
#include "rom.h"
#include "cext.h"
#include "misc.h"

#ifdef __CLOCK_EMULATE__
#define MPU_I2C_ADDRESS  0x44 //0100 0100
#else
#define MPU_I2C_ADDRESS  0xD2 //1101 0010
#endif

static unsigned char mpu_threshold;

static void mpu_load_config(void)
{
  CDBG("mpu_load_config\n");
  mpu_threshold = rom_read(ROM_FUSE_MPU);
  CDBG("mpu_threshold = %bu\n", mpu_threshold);
}

static void mpu_power_on(void)
{
  unsigned char val;
  CDBG("mpu_power_on\n");
  
  mpu_load_config();
  
#ifdef __CLOCK_EMULATE__
  // Configuration Register 设置为全1，用于input
  I2C_Put(MPU_I2C_ADDRESS, 0x3, 0xFF);
  
  // Polarity Inversion Register 设置为全0
  I2C_Put(MPU_I2C_ADDRESS, 0x2, 0x0);
  
  // 读取一次端口寄存器消除中断
  I2C_Get(MPU_I2C_ADDRESS, 0x0, &val);
  
  CDBG("mpu port reg is 0x%02bx\n", val);
#else
  /* Device reset */
  val = 0x80;
  I2C_Put(MPU_I2C_ADDRESS, 0x6B, val);
  delay_ms(100);
  
  /* Signal Path Reset */
  val = 0x07;
  I2C_Put(MPU_I2C_ADDRESS, 0x68, val);
  delay_ms(100);
  
  /* Wake up chip. */
  val = 0x00;
  I2C_Put(MPU_I2C_ADDRESS, 0x6B, val);
  delay_ms(100);
  
  /* PWR_MGMT_1 , disable temp */
  val = 0x08;
  I2C_Put(MPU_I2C_ADDRESS, 0x6B, val);
  delay_ms(100);

  /* PWR_MGMT_2, close gyro,LP_WAKE_CTRL = 1.25HZ */
  val = 0x07;
  I2C_Put(MPU_I2C_ADDRESS, 0x6C, val);
  delay_ms(100);
  
  /* Set sample rate */
  val = 0x07;
  I2C_Put(MPU_I2C_ADDRESS, 0x19, val);
  delay_ms(100);
  
  /* Set config, DLPF_CFG = 21 HZ */
  val = 0x04;
  I2C_Put(MPU_I2C_ADDRESS, 0x1A, val);
  delay_ms(100);
  
  /* Set the accel full-scale range to 2G, ACCEL_HPF to 0.63Hz */
  val = 0x04;
  I2C_Put(MPU_I2C_ADDRESS, 0x1C, val);
  delay_ms(100);
  
  mpu_threshold_reset();
  delay_ms(100);
  
  /* MOT_DUR to 1 */
  val = 0x01;
  I2C_Put(MPU_I2C_ADDRESS, 0x20, val);
  delay_ms(100);  
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
  val = 0xE0;
  I2C_Put(MPU_I2C_ADDRESS, 0x37, val);
  delay_ms(100);  
  /* Enter Accelerometer Only Low Power Mode */
  // 00101000 = 0x28
  // (i) Set CYCLE bit to 0
  // (ii) Set SLEEP bit to 0
  // (iii) Set TEMP_DIS bit to 1
  // 01000111 = 0x47
  // LP_WAKE_CTRL = 01 (5HZ)
  // STBY_XA = STBY_YA = STBY_ZA = 0
  // STBY_XG = STBY_YG = STBY_ZG = 1
  val = 0x08; 
  I2C_Put(MPU_I2C_ADDRESS, 0x6B, val);
  val = 0x47;
  I2C_Put(MPU_I2C_ADDRESS, 0x6C, val);
  delay_ms(100);
  
  // read int status to clr int
  I2C_Get(MPU_I2C_ADDRESS, 0x3A, &val);
  
  delay_ms(100);
  
  /* MOT_EN = 1 */
  val = 0x40;
  I2C_Put(MPU_I2C_ADDRESS, 0x38, val);  
  delay_ms(100);
  
  // read int status to clr int
  I2C_Get(MPU_I2C_ADDRESS, 0x3A, &val);
#endif
}

static void mpu_power_off(void)
{
  unsigned char val;
  CDBG("mpu_power_off\n");
#ifdef __CLOCK_EMULATE__
  // Configuration Register 设置为全0，用于output
  I2C_Put(MPU_I2C_ADDRESS, 0x3, 0xFF);
  
  // Polarity Inversion Register 设置为全0
  I2C_Put(MPU_I2C_ADDRESS, 0x2, 0x0);
  
  // 读取一次端口寄存器消除中断
  I2C_Get(MPU_I2C_ADDRESS, 0x0, &val);
  
  CDBG("mpu port reg is 0x%02bx\n", val);
#else
  
  /* Device reset */
  val = 0x80;
  I2C_Put(MPU_I2C_ADDRESS, 0x6B, val);
  delay_ms(100);
  
  /* Signal Path Reset */
  val = 0x07;
  I2C_Put(MPU_I2C_ADDRESS, 0x68, val);
  delay_ms(100);
  
  /* Wake up chip. */
  val = 0x00;
  I2C_Put(MPU_I2C_ADDRESS, 0x6B, val);
  delay_ms(100);
  
  /* PWR_MGMT_1 , disable temp */
  val = 0x08;
  I2C_Put(MPU_I2C_ADDRESS, 0x6B, val);
  delay_ms(100);

  /* PWR_MGMT_2, close gyro,LP_WAKE_CTRL = 1.25HZ */
  val = 0x07;
  I2C_Put(MPU_I2C_ADDRESS, 0x6C, val);
  delay_ms(100);
  
again:
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
  val = 0xE0;
  I2C_Put(MPU_I2C_ADDRESS, 0x37, val);
  delay_ms(100);

  // disable interrupt
  val = 0x00;
  I2C_Put(MPU_I2C_ADDRESS, 0x38, val);
  delay_ms(100);
  
  // read int status to clr int
  I2C_Get(MPU_I2C_ADDRESS, 0x3A, &val);
  delay_ms(100);
  
  // into sleep
  // 01001000 = 0x48
  // Set SLEEP bit to 1
  // 01111111 = 0x7F
  // LP_WAKE_CTRL = 01 (5HZ)
  // STBY_XA = STBY_YA = STBY_ZA = 1
  // STBY_XG = STBY_YG = STBY_ZG = 1
  val = 0x48;
  I2C_Put(MPU_I2C_ADDRESS, 0x6B, val);
  val = 0x7F;
  I2C_Put(MPU_I2C_ADDRESS, 0x6C, val);
  delay_ms(100);
  
  I2C_Get(MPU_I2C_ADDRESS, 0x37, &val);
  if(val != 0xE0)
    goto again;
#endif
}

void mpu_initialize (void)
{
  CDBG("mpu_initialize\n");
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
  unsigned char val;
  bit has_event = 0;
  
  CDBG("scan_mpu\n");
  val = mpu_read_int_status();
#ifdef __CLOCK_EMULATE__
  // 读取一次端口寄存器消除中断
  if ((val & 0x1) == 0) {
#else
  if ((val & 0x40) != 0) {  
#endif    
    CDBG("EV_MOT_MPU\n");
    set_task(EV_MOT_MPU);
    has_event = 1;
  }
  
  if(has_event && power_test_flag()) {
    power_clr_flag();
  }
}

void mpu_enable(bit enable)
{
  CDBG("mpu_enable %bu\n", enable ? 1 : 0 );
  if(enable) {
    mpu_power_on();
  } else {
    mpu_power_off();
  }
 
}

void mpu_threshold_reset(void)
{
  mpu_threshold_set(mpu_threshold);
}

// MPU_THRESHOLD_MIN -> MPU_THRESHOLD_MAX -> MPU_THRESHOLD_INVALID -> MPU_THRESHOLD_MIN
unsigned char mpu_threshold_inc(unsigned char thres)
{
  if(thres != MPU_THRESHOLD_INVALID) {
    if(thres < MPU_THRESHOLD_MAX) {
      thres ++;
      return thres;
    }else {
      return MPU_THRESHOLD_INVALID;
    }
  } else {
    return MPU_THRESHOLD_MIN;
  }
}

unsigned char mpu_threshold_get(void)
{
#ifdef __CLOCK_EMULATE__
  return mpu_threshold;
#else
  unsigned char val;
  I2C_Get(MPU_I2C_ADDRESS, 0x1F, &val);
  return val;
#endif
}

void mpu_threshold_set(unsigned char val)
{
  CDBG("mpu_threshold_set %bu\n", val);
#ifdef __CLOCK_EMULATE__
  return;
#else
  if(val != MPU_THRESHOLD_INVALID) {
    if(val > MPU_THRESHOLD_MAX) {
      val = MPU_THRESHOLD_MAX;
    }
    if(val < MPU_THRESHOLD_MIN) {
      val = MPU_THRESHOLD_MIN;
    }
  }
  I2C_Put(MPU_I2C_ADDRESS, 0x1F, val);
#endif
}


// 读取中断状态，清除中断
unsigned char mpu_read_int_status(void)
{
  unsigned char val;
#ifdef __CLOCK_EMULATE__
  I2C_Get(MPU_I2C_ADDRESS, 0x0, &val);
#else
  I2C_Get(MPU_I2C_ADDRESS, 0x3A, &val);
#endif
  CDBG("mpu int reg is 0x%02bx\n", val);
  return val;
}

