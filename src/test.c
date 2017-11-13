#include <STC89C5xRC.H>
#include <stdio.h>
#include "i2c.h"
#include "com.h"
#include "debug.h"
#include "int_hub.h"


// PCA9535 logic, 实际上可能会用TCA9535
#define SERIAL_HUB_I2C_ADDR 0x40 //0100 0000

void serial_hub_initialize(void);
void serial_ctl_out(void);
void serial_set_ctl_bit(unsigned char index, bit what);
bit serial_test_ctl_bit(unsigned char index);

enum serial_hub_crl_bits{
  SERIAL_BIT_FUSE_EN = 0,
  SERIAL_BIT_HG_EN = 1,  
  SERIAL_BIT_GYRO_EN = 2, 
  SERIAL_BIT_THERMO_EN = 3,
  SERIAL_BIT_TRIPWIRE_EN = 4,  
  SERIAL_BIT_FUSE0_TRIGGER = 5, 
  SERIAL_BIT_FUSE1_TRIGGER = 6,
  SERIAL_BIT_UNUSE0 = 7, 
  SERIAL_BIT_UNUSE1 = 8, 
  SERIAL_BIT_UNUSE2 = 9,  
  SERIAL_BIT_UNUSE3 = 10,
  SERIAL_BIT_FUSE0_BROKE_TEST = 11,  
  SERIAL_BIT_FUSE0_SHORT_TEST = 12, 
  SERIAL_BIT_FUSE1_BROKE_TEST = 13, 
  SERIAL_BIT_FUSE1_SHORT_TEST = 14,  
  SERIAL_BIT_TRIPWIRE_TEST = 15,
};

static unsigned char ctl_buffer[2];

static void serial_dump_ctl(void)
{
  CDBG("++++++serial_dump_ctl begin++++++\n");
  CDBG("ctl_buffer[0] = %bx ctl_buffer[1] = %bx\n", ctl_buffer[0], ctl_buffer[1]);
  CDBG("%c %s\n", serial_test_ctl_bit(SERIAL_BIT_TRIPWIRE_TEST) ? '1' : '0', "SERIAL_BIT_TRIPWIRE_TEST");
  CDBG("%c %s\n", serial_test_ctl_bit(SERIAL_BIT_FUSE1_SHORT_TEST) ? '1' : '0', "SERIAL_BIT_FUSE1_SHORT_TEST");
  CDBG("%c %s\n", serial_test_ctl_bit(SERIAL_BIT_FUSE1_BROKE_TEST) ? '1' : '0', "SERIAL_BIT_FUSE1_BROKE_TEST");
  CDBG("%c %s\n", serial_test_ctl_bit(SERIAL_BIT_FUSE0_SHORT_TEST) ? '1' : '0', "SERIAL_BIT_FUSE0_SHORT_TEST");
  CDBG("%c %s\n", serial_test_ctl_bit(SERIAL_BIT_FUSE0_BROKE_TEST) ? '1' : '0', "SERIAL_BIT_FUSE0_BROKE_TEST");
  CDBG("%c %s\n", serial_test_ctl_bit(SERIAL_BIT_UNUSE3) ? '1' : '0', "SERIAL_BIT_UNUSE3");
  CDBG("%c %s\n", serial_test_ctl_bit(SERIAL_BIT_UNUSE2) ? '1' : '0', "SERIAL_BIT_UNUSE2");
  CDBG("%c %s\n", serial_test_ctl_bit(SERIAL_BIT_UNUSE1) ? '1' : '0', "SERIAL_BIT_UNUSE1");
  CDBG("%c %s\n", serial_test_ctl_bit(SERIAL_BIT_UNUSE0) ? '1' : '0', "SERIAL_BIT_UNUSE0");
  CDBG("%c %s\n", serial_test_ctl_bit(SERIAL_BIT_FUSE1_TRIGGER) ? '1' : '0', "SERIAL_BIT_FUSE1_TRIGGER");
  CDBG("%c %s\n", serial_test_ctl_bit(SERIAL_BIT_FUSE0_TRIGGER) ? '1' : '0', "SERIAL_BIT_FUSE0_TRIGGER");
  CDBG("%c %s\n", serial_test_ctl_bit(SERIAL_BIT_TRIPWIRE_EN) ? '1' : '0', "SERIAL_BIT_TRIPWIRE_EN");
  CDBG("%c %s\n", serial_test_ctl_bit(SERIAL_BIT_THERMO_EN) ? '1' : '0', "SERIAL_BIT_THERMO_EN");
  CDBG("%c %s\n", serial_test_ctl_bit(SERIAL_BIT_GYRO_EN) ? '1' : '0', "SERIAL_BIT_GYRO_EN");
  CDBG("%c %s\n", serial_test_ctl_bit(SERIAL_BIT_HG_EN) ? '1' : '0', "SERIAL_BIT_HG_EN");
  CDBG("%c %s\n", serial_test_ctl_bit(SERIAL_BIT_FUSE_EN) ? '1' : '0', "SERIAL_BIT_FUSE_EN");
  CDBG("++++++serial_dump_ctl end++++++++\n");
}



void serial_ctl_out(void)
{
  I2C_Put(SERIAL_HUB_I2C_ADDR, 0x2, ctl_buffer[0]);
  I2C_Put(SERIAL_HUB_I2C_ADDR, 0x3, ctl_buffer[1]);
  
  serial_dump_ctl();
}

void serial_set_ctl_bit(unsigned char index, bit what)
{
  if(index > sizeof(ctl_buffer) * 8)
    return;
  
  ctl_buffer[index / 8] &= ~(1<<(index % 8));
  if(what)
    ctl_buffer[index / 8] |= 1<<(index % 8);
}

bit serial_test_ctl_bit(unsigned char index)
{
  if(index > sizeof(ctl_buffer) * 8)
    return 0;
  
  return (ctl_buffer[index / 8] & (1<<(index % 8))) == 0 ? 0 : 1;
}


void serial_hub_initialize(void)
{
  CDBG("serial_hub_initialize\n");

  ctl_buffer[0] = ctl_buffer[1] = 0xFF;
  
  // Configuration Register 设置为全0，用于output
  I2C_Put(SERIAL_HUB_I2C_ADDR, 0x6, 0x0);
  
  // Polarity Inversion Register 设置为全0
  I2C_Put(SERIAL_HUB_I2C_ADDR, 0x7, 0x0);
  
  // Configuration Register 设置为全0，用于output
  I2C_Put(SERIAL_HUB_I2C_ADDR, 0x4, 0x0);
  
  // Polarity Inversion Register 设置为全0
  I2C_Put(SERIAL_HUB_I2C_ADDR, 0x5, 0x0);
  
  serial_ctl_out();
}

void main(void)
{
  char buf[5];
  bit a = 1;
  
  EA = 1;
  
  com_initialize();
  serial_hub_initialize();
  
  do {
    gets (buf, sizeof(buf)-1);
    printf ("Input string \"%s\"", buf);
    printf("bit a is %bx\n", a == 0 ? 0 : 1);
    serial_set_ctl_bit(SERIAL_BIT_FUSE_EN, a);
    a = ~a;
    serial_ctl_out();
  } while (buf [0] != '\0');
  
}