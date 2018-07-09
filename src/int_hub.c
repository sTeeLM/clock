#include <STC89C5xRC.H>
#include <stdio.h>
#include "i2c.h"
#include "int_hub.h"
#include "serial_hub.h"
#include "debug.h"
#include "power.h"
#include "rtc.h"
#include "beeper.h"
#include "task.h"
#include "hg.h"
#include "mpu.h"
#include "thermo.h"
#include "fuse.h"
#include "remote.h"
#include "power.h"
#include "indicator.h"
#include "cext.h"



sbit RTC_INT        = P1 ^ 1;
sbit MPU_INT        = P1 ^ 2;
sbit POWER_INT      = P1 ^ 3;
sbit EXT_INT        = P1 ^ 4;
sbit INT_BIT        = P3 ^ 3;


// PCA9535 logic, 实际上可能会用TCA9535
#define INT_HUB_I2C_ADDR 0x42 //0100 0010


static void int1_ISR (void) interrupt 2 using 1
{
  IE1 = 0; // 清除中断标志位
  set_task(EV_SCAN_INT_HUB);
}

void int_hub_initialize (void)
{
  unsigned char val;
  CDBG("int_hub_initialize\n");
  // Configuration Register 设置为全1，用于input
  I2C_Put(INT_HUB_I2C_ADDR, 0x6, 0xFF);
  // Polarity Inversion Register 设置为全0
  I2C_Put(INT_HUB_I2C_ADDR, 0x4, 0x0);


  // 读取一次端口寄存器消除中断
  I2C_Get(INT_HUB_I2C_ADDR, 0x0, &val);
  CDBG("int hub 0 port reg is %02bx\n", val);
  
  // Configuration Register 设置为全1，用于input
  I2C_Put(INT_HUB_I2C_ADDR, 0x7, 0xFF);
  // Polarity Inversion Register 设置为全0
  I2C_Put(INT_HUB_I2C_ADDR, 0x5, 0x0);

  // 读取一次端口寄存器消除中断
  I2C_Get(INT_HUB_I2C_ADDR, 0x1, &val);
  CDBG("int hub 1 port reg is %02bx\n", val);
  
  RTC_INT = 1;
  MPU_INT   = 1;
  EXT_INT    = 1;
  INT_BIT    = 1;
  
  IT1 = 1; // 设置为边沿触发
  EX1 = 1; // 开中断
}


bit int_hub_test_bit(unsigned char index, unsigned int status)
{
  if(index > sizeof(status) * 8)
    return 0;
  
  return (status & (1<<(index % 16))) == 0 ? 0 : 1;
}

void int_hub_dump(void)
{
  //RTC_INT || !EXT_INT || !THERMO_INT || !MPU_INT
  CDBG("++++++int_hub_dump begin++++++\n");
  CDBG("[INT] %c\n", INT_BIT ? '1' : '0');
  CDBG("[RTC_INT] %c\n", RTC_INT ? '1' : '0');
  CDBG("[MPU_INT] %c\n", MPU_INT ? '1' : '0'); 
  CDBG("[POWER_INT] %c\n", POWER_INT ? '1' : '0'); 
  CDBG("[EXT_INT] %c\n", EXT_INT ? '1' : '0'); 
  CDBG("++++++int_hub_dump end++++++\n");
}

unsigned int int_hub_get_status(void)
{
  unsigned int status = 0;
  unsigned char val;

  I2C_Get(INT_HUB_I2C_ADDR, 0x1, &val); 
  CDBG("int_hub_get_status hi is 0x%02bx\n", val);
  status = val;
  
  status = status << 8;
  I2C_Get(INT_HUB_I2C_ADDR, 0x0, &val);
  CDBG("int_hub_get_status lo is 0x%02bx\n", val);
  status |= val;
  
  return status;
}

void int_hub_dump_ext_status(unsigned int status)
{
  CDBG("++++++int_hub_dump_ext_status begin++++++\n");
  CDBG("[%02bu] %c %s\n", INT_HUB_FUSE0_BROKE, int_hub_test_bit(INT_HUB_FUSE0_BROKE, status) ? '1' : '0', "INT_HUB_FUSE0_BROKE");
  CDBG("[%02bu] %c %s\n", INT_HUB_FUSE1_BROKE, int_hub_test_bit(INT_HUB_FUSE1_BROKE, status) ? '1' : '0', "INT_HUB_FUSE1_BROKE");
  CDBG("[%02bu] %c %s\n", INT_HUB_HG0_HIT, int_hub_test_bit(INT_HUB_HG0_HIT, status) ? '1' : '0', "INT_HUB_HG0_HIT");
  CDBG("[%02bu] %c %s\n", INT_HUB_HG1_HIT, int_hub_test_bit(INT_HUB_HG1_HIT, status) ? '1' : '0', "INT_HUB_HG1_HIT");
  CDBG("[%02bu] %c %s\n", INT_HUB_HG2_HIT, int_hub_test_bit(INT_HUB_HG2_HIT, status) ? '1' : '0', "INT_HUB_HG2_HIT");
  CDBG("[%02bu] %c %s\n", INT_HUB_HG3_HIT, int_hub_test_bit(INT_HUB_HG3_HIT, status) ? '1' : '0', "INT_HUB_HG3_HIT"); 
  CDBG("[%02bu] %c %s\n", INT_HUB_TRIPWIRE_HIT, int_hub_test_bit(INT_HUB_TRIPWIRE_HIT, status) ? '1' : '0', "INT_HUB_TRIPWIRE_HIT");
  CDBG("[%02bu] %c %s\n", INT_HUB_THERMO_HI_HIT, int_hub_test_bit(INT_HUB_THERMO_HI_HIT, status) ? '1' : '0', "INT_HUB_THERMO_HI_HIT");
  CDBG("[%02bu] %c %s\n", INT_HUB_THERMO_LO_HIT, int_hub_test_bit(INT_HUB_THERMO_LO_HIT, status) ? '1' : '0', "INT_HUB_THERMO_LO_HIT");
  CDBG("[%02bu] %c %s\n", INT_HUB_REMOTE_DISARM, int_hub_test_bit(INT_HUB_REMOTE_DISARM, status) ? '1' : '0', "INT_HUB_REMOTE_DISARM");
  CDBG("[%02bu] %c %s\n", INT_HUB_REMOTE_DETONATE, int_hub_test_bit(INT_HUB_REMOTE_DETONATE, status) ? '1' : '0', "INT_HUB_REMOTE_DETONATE");
  CDBG("[%02bu] %c %s\n", INT_HUB_UNSUSED0, int_hub_test_bit(INT_HUB_UNSUSED0, status) ? '1' : '0', "INT_HUB_UNSUSED0");
  CDBG("[%02bu] %c %s\n", INT_HUB_UNSUSED1, int_hub_test_bit(INT_HUB_UNSUSED1, status) ? '1' : '0', "INT_HUB_UNSUSED1");  
  CDBG("[%02bu] %c %s\n", INT_HUB_UNSUSED2, int_hub_test_bit(INT_HUB_UNSUSED2, status) ? '1' : '0', "INT_HUB_UNSUSED2");
  CDBG("[%02bu] %c %s\n", INT_HUB_UNSUSED3, int_hub_test_bit(INT_HUB_UNSUSED3, status) ? '1' : '0', "INT_HUB_UNSUSED3");
  CDBG("[%02bu] %c %s\n", INT_HUB_UNSUSED4, int_hub_test_bit(INT_HUB_UNSUSED4, status) ? '1' : '0', "INT_HUB_UNSUSED4");
  CDBG("++++++int_hub_dump_ext_status ends++++++\n");
}
// 中断全部是下降边缘触发，在每一个scan函数里保证能清中断
void scan_int_hub_proc (enum task_events ev)
{
  unsigned int status = 0;
  CDBG("scan_int_hub_proc\n");
  
  UNUSED_PARAM(ev);
  
  int_hub_dump();

  if(!RTC_INT) {
    scan_rtc(); // call scan_alarm or scan_lt_timer
  }

  
  if(!EXT_INT) {
    /*
    I2C_Get(INT_HUB_I2C_ADDR, 0x1, &val); 
    status = val; 
    status = status << 8;
    I2C_Get(INT_HUB_I2C_ADDR, 0x0, &val);
    status |= val;
    */
    status  = int_hub_get_status();
    int_hub_dump_ext_status(status);
    scan_hg(status);
    scan_fuse(status);
    scan_thermo(status); 
    scan_remote(status);    
  }
  
  if(!MPU_INT) {
    scan_mpu();  
  }
  
  if(!POWER_INT) {
    scan_power();
  }
/*  
  if(!RTC_INT || !EXT_INT || !MPU_INT ) {
    set_task(EV_SCAN_INT_HUB);
  }
*/
}
