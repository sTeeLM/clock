#include <STC89C5xRC.H>
#include <string.h>
#include "serial_hub.h"
#include "debug.h"
#include "misc.h"


// 由于脚不够，用串并转的方法进行扩展

sbit SR_DATA_IN = P3 ^ 5;
sbit SR_WR      = P3 ^ 6;  // 上升沿有效
sbit SR_WCLK    = P3 ^ 7;  // 上升沿有效

static unsigned char ctl_buffer[3];

static void serial_dump_ctl(void)
{
  /*
  CDBG("++++++serial_dump_ctl begin++++++\n");
  CDBG("%c %s\n", serial_test_ctl_bit(SERIAL_BIT_C_UNUSED0) ? '1' : '0', "SERIAL_BIT_C_UNUSED0");
  CDBG("%c %s\n", serial_test_ctl_bit(SERIAL_BIT_C_UNUSED1) ? '1' : '0', "SERIAL_BIT_C_UNUSED1");
  CDBG("%c %s\n", serial_test_ctl_bit(SERIAL_BIT_C_UNUSED2) ? '1' : '0', "SERIAL_BIT_C_UNUSED2");
  CDBG("%c %s\n", serial_test_ctl_bit(SERIAL_BIT_C_UNUSED3) ? '1' : '0', "SERIAL_BIT_C_UNUSED3");
  CDBG("%c %s\n", serial_test_ctl_bit(SERIAL_BIT_C_UNUSED4) ? '1' : '0', "SERIAL_BIT_C_UNUSED4");
  CDBG("%c %s\n", serial_test_ctl_bit(SERIAL_BIT_C_UNUSED5) ? '1' : '0', "SERIAL_BIT_C_UNUSED5");
  CDBG("%c %s\n", serial_test_ctl_bit(SERIAL_BIT_C_UNUSED6) ? '1' : '0', "SERIAL_BIT_C_UNUSED6");
  CDBG("%c %s\n", serial_test_ctl_bit(SERIAL_BIT_C_UNUSED7) ? '1' : '0', "SERIAL_BIT_C_UNUSED7");
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
  */
}



void serial_ctl_out(void)
{
  unsigned char i,j, b;
  for(i = 0 ; i < sizeof(ctl_buffer) ; i ++) {
    b = ctl_buffer[i];
    for(j = 0 ; j < 8 ; j ++) {
      SR_DATA_IN = b & 0x1;
      b >>= 1;
      SR_WCLK = 0;
      delay_5us(1);
      SR_WCLK = 1;
    }
  }
  
  SR_WR = 0;
  delay_5us(1);
  SR_WR = 1;
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
  SR_WR   = 1;
  SR_WCLK = 1;
  SR_DATA_IN  = 1;
  memset(ctl_buffer, 0, sizeof(ctl_buffer));
  
  serial_ctl_out();
}