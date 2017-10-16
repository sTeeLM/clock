#include <STC89C5xRC.H>
#include "i2c.h"
#include "int_hub.h"
#include "serial_hub.h"
#include "debug.h"
#include "power.h"
#include "rtc.h"
#include "beeper.h"
#include "task.h"
#include "hg.h"
#include "gyro.h"
#include "thermo.h"
#include "tripwire.h"
#include "fuse.h"
#include "cext.h"



sbit RTC_INT        = P1 ^ 1;
sbit GYRO_INT       = P1 ^ 2;
sbit THERMO_INT     = P1 ^ 3;
sbit EXT0_INT       = P1 ^ 4;
sbit EXT1_INT       = P3 ^ 5;
sbit INT_BIT        = P3 ^ 3;

// PCA9554 logic, ʵ���Ͽ��ܻ���PCA9534

#define INT_HUB0_I2C_ADDR 0x40 //0100 0000
#define INT_HUB1_I2C_ADDR 0x42 //0100 0010


static void int1_ISR (void) interrupt 2 using 1
{
  IE1 = 0; // ����жϱ�־λ
  set_task(EV_SCAN_INT_HUB);
}

void int_hub_initialize (void)
{
  unsigned char val;
  CDBG("int_hub_initialize\n");
  // Configuration Register ����Ϊȫ1������input
  I2C_Put(INT_HUB0_I2C_ADDR, 0x3, 0xFF);
  // Polarity Inversion Register ����Ϊȫ0
  I2C_Put(INT_HUB0_I2C_ADDR, 0x2, 0x0);
  // ��ȡһ�ζ˿ڼĴ��������ж�
  I2C_Get(INT_HUB0_I2C_ADDR, 0x0, &val);
  CDBG("int hub 0 port reg is %bx\n", val);
  
  // Configuration Register ����Ϊȫ1������input
  I2C_Put(INT_HUB1_I2C_ADDR, 0x3, 0xFF);
  // Polarity Inversion Register ����Ϊȫ0
  I2C_Put(INT_HUB1_I2C_ADDR, 0x2, 0x0);
  // ��ȡһ�ζ˿ڼĴ��������ж�
  I2C_Get(INT_HUB1_I2C_ADDR, 0x0, &val);
  CDBG("int hub 1 port reg is %bx\n", val);
  
  INT_BIT = 1;
  
  IT1 = 1; // ����Ϊ���ش���
  EX1 = 1; // ���ж�
}


bit int_hub_test_bit(unsigned char index, unsigned int status)
{
  if(index > sizeof(status) * 8)
    return 0;
  
  return (status & (1<<(index % 16))) == 0 ? 0 : 1;
}

static void int_hub_dump_status(unsigned int status)
{
  UNUSED_PARAM(status);
  /*
  CDBG("++++++int_hub_dump_status begin++++++\n");
  CDBG("%c %s\n", int_hub_test_bit(INT_HUB_FUSE0_SHORT, status) ? '1' : '0', "INT_HUB_FUSE0_SHORT");
  CDBG("%c %s\n", int_hub_test_bit(INT_HUB_FUSE0_BROKE, status) ? '1' : '0', "INT_HUB_FUSE0_BROKE");
  CDBG("%c %s\n", int_hub_test_bit(INT_HUB_FUSE1_SHORT, status) ? '1' : '0', "INT_HUB_FUSE1_SHORT");
  CDBG("%c %s\n", int_hub_test_bit(INT_HUB_FUSE1_BROKE, status) ? '1' : '0', "INT_HUB_FUSE1_BROKE");
  CDBG("%c %s\n", int_hub_test_bit(INT_HUB_UNSUSED0, status) ? '1' : '0', "INT_HUB_UNSUSED0");
  CDBG("%c %s\n", int_hub_test_bit(INT_HUB_UNSUSED1, status) ? '1' : '0', "INT_HUB_UNSUSED1");
  CDBG("%c %s\n", int_hub_test_bit(INT_HUB_UNSUSED2, status) ? '1' : '0', "INT_HUB_UNSUSED2");
  CDBG("%c %s\n", int_hub_test_bit(INT_HUB_UNSUSED3, status) ? '1' : '0', "INT_HUB_UNSUSED3");
  CDBG("%c %s\n", int_hub_test_bit(INT_HUB_HG0_HIT, status) ? '1' : '0', "INT_HUB_HG0_HIT");
  CDBG("%c %s\n", int_hub_test_bit(INT_HUB_HG1_HIT, status) ? '1' : '0', "INT_HUB_HG1_HIT");
  CDBG("%c %s\n", int_hub_test_bit(INT_HUB_HG2_HIT, status) ? '1' : '0', "INT_HUB_HG2_HIT");
  CDBG("%c %s\n", int_hub_test_bit(INT_HUB_HG3_HIT, status) ? '1' : '0', "INT_HUB_HG3_HIT"); 
  CDBG("%c %s\n", int_hub_test_bit(INT_HUB_TRIPWIRE_HIT, status) ? '1' : '0', "INT_HUB_TRIPWIRE_HIT");  
  CDBG("%c %s\n", int_hub_test_bit(INT_HUB_UNSUSED4, status) ? '1' : '0', "INT_HUB_UNSUSED4");
  CDBG("%c %s\n", int_hub_test_bit(INT_HUB_UNSUSED5, status) ? '1' : '0', "INT_HUB_UNSUSED5");
  CDBG("%c %s\n", int_hub_test_bit(INT_HUB_UNSUSED6, status) ? '1' : '0', "INT_HUB_UNSUSED6");
  CDBG("++++++int_hub_dump_status ends++++++\n");
  */
}

void scan_int_hub_proc (enum task_events ev)
{
  unsigned int status = 0;
  unsigned char val;
  CDBG("scan_int_hub_proc\n");
  
  UNUSED_PARAM(ev);
  

  if(!RTC_INT) {
    scan_rtc(); // call scan_alarm or scan_lt_timer
  }

  if(!EXT0_INT) {
    I2C_Get(INT_HUB0_I2C_ADDR, 0x0, &val);
    status = val;
    int_hub_dump_status(status);
    scan_fuse(status);
  }
  
  if(!EXT1_INT) {
    I2C_Get(INT_HUB1_I2C_ADDR, 0x0, &val); 
    status = val; 
    status = status << 8;
    int_hub_dump_status(status);
    scan_hg(status);
    scan_tripwire(status);
  }

  
  if(!THERMO_INT) {
    scan_thermo();
  }
  
  if(!GYRO_INT) {
    scan_gyro();  
  }
  
  if(!RTC_INT || !EXT0_INT || !EXT1_INT|| !THERMO_INT || !GYRO_INT ) {
    set_task(EV_SCAN_INT_HUB);
  }
 
}