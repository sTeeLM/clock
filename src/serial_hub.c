#include <STC89C5xRC.H>
#include <string.h>
#include "serial_hub.h"
#include "i2c.h"
#include "debug.h"
#include "misc.h"


// PCA9554 logic, 实际上可能会用PCA9534

#define SERIAL_HUB0_I2C_ADDR 0x70 //0111 0000
#define SERIAL_HUB1_I2C_ADDR 0x72 //0111 0010


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
  CDBG("%c %s\n", serial_test_ctl_bit(SERIAL_BIT_HG3_FIX) ? '1' : '0', "SERIAL_BIT_HG3_FIX");
  CDBG("%c %s\n", serial_test_ctl_bit(SERIAL_BIT_HG2_FIX) ? '1' : '0', "SERIAL_BIT_HG2_FIX");
  CDBG("%c %s\n", serial_test_ctl_bit(SERIAL_BIT_HG1_FIX) ? '1' : '0', "SERIAL_BIT_HG1_FIX");
  CDBG("%c %s\n", serial_test_ctl_bit(SERIAL_BIT_HG0_FIX) ? '1' : '0', "SERIAL_BIT_HG0_FIX");
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
  I2C_Put(SERIAL_HUB0_I2C_ADDR, 0x1, ctl_buffer[0]);
  I2C_Put(SERIAL_HUB1_I2C_ADDR, 0x1, ctl_buffer[1]);
  
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
  I2C_Put(SERIAL_HUB0_I2C_ADDR, 0x3, 0x0);
  
  // Polarity Inversion Register 设置为全0
  I2C_Put(SERIAL_HUB0_I2C_ADDR, 0x2, 0x0);
  
  // Configuration Register 设置为全0，用于output
  I2C_Put(SERIAL_HUB1_I2C_ADDR, 0x3, 0x0);
  
  // Polarity Inversion Register 设置为全0
  I2C_Put(SERIAL_HUB1_I2C_ADDR, 0x2, 0x0);
  
  serial_ctl_out();
}