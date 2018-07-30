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
#define MPU_I2C_ADDRESS  0xA6 //1010 0110
#endif

static unsigned char mpu_threshold;
static bit mpu_enabled;

static void mpu_load_config(void)
{
  CDBG("mpu_load_config\n");
  mpu_threshold = rom_read(ROM_FUSE_MPU);
  CDBG("mpu_threshold = %bu\n", mpu_threshold);
}

static void mpu_device_init(void)
{
  mpu_load_config();
  
#ifndef __CLOCK_EMULATE__
  // Register 0x31—DATA_FORMAT (Read/Write)
  // INT_INVERT = 1， interrupts to active low
  // Range Bits = 00， +-2G
  I2C_Put(MPU_I2C_ADDRESS, 0x31, 0x20);

  // Register 0x2E—INT_ENABLE (Read/Write), all DISABLE
  I2C_Put(MPU_I2C_ADDRESS, 0x2E, 0);
  
  // Register 0x2F—INT_MAP (R/W), all to INT1
  I2C_Put(MPU_I2C_ADDRESS, 0x2F, 0);
  
  // THRESH_ACT (Read/Write)
  mpu_threshold_set(mpu_threshold);
  
  //Register 0x27—ACT_INACT_CTL (Read/Write)
  // ACT ac/dc = 1 (ac)
  // ACT_X enable = 1
  // ACT_X enable = 1
  // ACT_Y enable = 1
  // INACT ac/dc = 0 (ac)
  // INACT_X enable = 0
  // INACT_Y enable = 0
  // INACT_Z enable = 0
  I2C_Put(MPU_I2C_ADDRESS, 0x27, 0xF0);
  
  // Register 0x25—THRESH_INACT (Read/Write) -> disable
  I2C_Put(MPU_I2C_ADDRESS, 0x25, 0);
  
  // Register 0x26—TIME_INACT (Read/Write) -> disable
  I2C_Put(MPU_I2C_ADDRESS, 0x26, 0);

  // Register 0x2D—POWER_CTL (Read/Write) into stand by
  // 00
  // Link = 0
  // AUTO_SLEEP = 0
  // Measure = 0
  // Sleep = 1
  // Wakeup = 00
  I2C_Put(MPU_I2C_ADDRESS, 0x2D, 0x40);
  
  mpu_read_int_status();
#endif
}

static void mpu_power_on(void)
{
  unsigned char val;
  CDBG("mpu_power_on\n");
  
#ifdef __CLOCK_EMULATE__
  // Configuration Register 设置为全1，用于input
  val = 0xFF;
  I2C_Put(MPU_I2C_ADDRESS, 0x3, val);
  
  // Polarity Inversion Register 设置为全0
  val = 0;
  I2C_Put(MPU_I2C_ADDRESS, 0x2, val);
  
  // 读取一次端口寄存器消除中断
  val = mpu_read_int_status();
  
  CDBG("mpu port reg is 0x%02bx\n", val);
#else
  mpu_load_config();
  
  mpu_threshold_set(mpu_threshold);
  
  // Register 0x2E—INT_ENABLE (Read/Write), enable Activity
  val = 0x10;
  I2C_Put(MPU_I2C_ADDRESS, 0x2E, val);
  
  // Register 0x2D—POWER_CTL (Read/Write), don't into autosleep
  // 00
  // Link = 0
  // AUTO_SLEEP = 0
  // Measure Bit = 1
  // Sleep = 0
  // Wakeup = 00
  val = 0x08;
  I2C_Put(MPU_I2C_ADDRESS, 0x2D, val);
  
  val = mpu_read_int_status();
  
#endif
  mpu_enabled = 1;
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
  val = mpu_read_int_status();
  
  CDBG("mpu port reg is 0x%02bx\n", val);
#else
  // Register 0x2E—INT_ENABLE (Read/Write),disable all
  val = 0;
  I2C_Put(MPU_I2C_ADDRESS, 0x2E, val);
  
  // Register 0x2D—POWER_CTL (Read/Write) into stand by
  // 00
  // Link = 0
  // AUTO_SLEEP = 0
  // Measure = 0
  // Sleep = 1
  // Wakeup = 00
  val = 0x40;
  I2C_Put(MPU_I2C_ADDRESS, 0x2D, val);
  
  mpu_read_int_status();
  
#endif
  mpu_enabled = 0;
}

void mpu_initialize (void)
{
  CDBG("mpu_initialize\n");
  mpu_device_init();
  mpu_power_off();
}

void mpu_enter_powersave(void)
{
}

void mpu_leave_powersave(void)
{
  
}

void scan_mpu(void)
{
  unsigned char val;
  bit has_event = 0;
  
  CDBG("scan_mpu\n");
  val = mpu_read_int_status();
#ifdef __CLOCK_EMULATE__
  // 读取一次端口寄存器消除中断
  if ((val & 0x1) == 0 && mpu_enabled) {
#else
  if ((val & 0x80) != 0 && mpu_enabled) {  
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
  if(enable && !mpu_enabled) {
    mpu_power_on();
  } else if(!enable && mpu_enabled){
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
  I2C_Get(MPU_I2C_ADDRESS, 0x24, &val);
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
  I2C_Put(MPU_I2C_ADDRESS, 0x24, val);
#endif
}


// 读取中断状态，清除中断
unsigned char mpu_read_int_status(void)
{
  unsigned char val;
#ifdef __CLOCK_EMULATE__
  I2C_Get(MPU_I2C_ADDRESS, 0x0, &val);
#else
  I2C_Get(MPU_I2C_ADDRESS, 0x30, &val);
#endif
  CDBG("mpu int reg is 0x%02bx\n", val);
  return val;
}

