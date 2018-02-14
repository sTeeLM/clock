#include <STC89C5xRC.H>
#include <stdio.h>
#include <string.h>
#include "serial_hub.h"
#include "i2c.h"
#include "debug.h"
#include "misc.h"


// PCA9535 logic, 实际上可能会用TCA9535
#define SERIAL_HUB_I2C_ADDR 0x40 //0100 0000

static unsigned char ctl_buffer[2];

void serial_dump(void)
{
  CDBG("++++++serial_dump begin++++++\n");
  CDBG("ctl_buffer[0] = %bx ctl_buffer[1] = %bx\n", ctl_buffer[0], ctl_buffer[1]);
  CDBG("[%02bd] %c %s\n", SERIAL_BIT_UNUSE6, serial_test_ctl_bit(SERIAL_BIT_UNUSE6) ? '1' : '0', "SERIAL_BIT_UNUSE6");
  CDBG("[%02bd] %c %s\n", SERIAL_BIT_UNUSE5, serial_test_ctl_bit(SERIAL_BIT_UNUSE5) ? '1' : '0', "SERIAL_BIT_UNUSE5");
  CDBG("[%02bd] %c %s\n", SERIAL_BIT_UNUSE4, serial_test_ctl_bit(SERIAL_BIT_UNUSE4) ? '1' : '0', "SERIAL_BIT_UNUSE4");
  CDBG("[%02bd] %c %s\n", SERIAL_BIT_UNUSE3, serial_test_ctl_bit(SERIAL_BIT_UNUSE3) ? '1' : '0', "SERIAL_BIT_UNUSE3");
  CDBG("[%02bd] %c %s\n", SERIAL_BIT_UNUSE2, serial_test_ctl_bit(SERIAL_BIT_UNUSE2) ? '1' : '0', "SERIAL_BIT_UNUSE2");
  CDBG("[%02bd] %c %s\n", SERIAL_BIT_UNUSE1, serial_test_ctl_bit(SERIAL_BIT_UNUSE1) ? '1' : '0', "SERIAL_BIT_UNUSE1");  
  CDBG("[%02bd] %c %s\n", SERIAL_BIT_TRIPWIRE_TEST, serial_test_ctl_bit(SERIAL_BIT_TRIPWIRE_TEST) ? '1' : '0', "SERIAL_BIT_TRIPWIRE_TEST");
  CDBG("[%02bd] %c %s\n", SERIAL_BIT_FUSE1_BROKE_TEST, serial_test_ctl_bit(SERIAL_BIT_FUSE1_BROKE_TEST) ? '1' : '0', "SERIAL_BIT_FUSE1_BROKE_TEST");
  CDBG("[%02bd] %c %s\n", SERIAL_BIT_FUSE0_BROKE_TEST, serial_test_ctl_bit(SERIAL_BIT_FUSE0_BROKE_TEST) ? '1' : '0', "SERIAL_BIT_FUSE0_BROKE_TEST");
  CDBG("[%02bd] %c %s\n", SERIAL_BIT_FUSE1_TRIGGER, serial_test_ctl_bit(SERIAL_BIT_FUSE1_TRIGGER) ? '1' : '0', "SERIAL_BIT_FUSE1_TRIGGER");
  CDBG("[%02bd] %c %s\n", SERIAL_BIT_FUSE0_TRIGGER, serial_test_ctl_bit(SERIAL_BIT_FUSE0_TRIGGER) ? '1' : '0', "SERIAL_BIT_FUSE0_TRIGGER");
  CDBG("[%02bd] %c %s\n", SERIAL_BIT_UNUSE0, serial_test_ctl_bit(SERIAL_BIT_UNUSE0) ? '1' : '0', "SERIAL_BIT_UNUSE0");
  CDBG("[%02bd] %c %s\n", SERIAL_BIT_THERMO_EN, serial_test_ctl_bit(SERIAL_BIT_THERMO_EN) ? '1' : '0', "SERIAL_BIT_THERMO_EN");
  CDBG("[%02bd] %c %s\n", SERIAL_BIT_GYRO_EN, serial_test_ctl_bit(SERIAL_BIT_GYRO_EN) ? '1' : '0', "SERIAL_BIT_GYRO_EN");
  CDBG("[%02bd] %c %s\n", SERIAL_BIT_HG_EN, serial_test_ctl_bit(SERIAL_BIT_HG_EN) ? '1' : '0', "SERIAL_BIT_HG_EN");
  CDBG("[%02bd] %c %s\n", SERIAL_BIT_FUSE_EN, serial_test_ctl_bit(SERIAL_BIT_FUSE_EN) ? '1' : '0', "SERIAL_BIT_FUSE_EN");
  CDBG("++++++serial_dump end++++++++\n");
}

void serial_ctl_out(void)
{
  I2C_Put(SERIAL_HUB_I2C_ADDR, 0x2, ctl_buffer[0]);
  I2C_Put(SERIAL_HUB_I2C_ADDR, 0x3, ctl_buffer[1]);
  
  serial_dump();
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