#ifndef __CLOCK_SERIAL_HUB_H__
#define __CLOCK_SERIAL_HUB_H__


enum serial_hub_crl_bits{
  SERIAL_BIT_FUSE_EN = 0,
  SERIAL_BIT_HG_EN = 1,  
  SERIAL_BIT_GYRO_EN = 2, 
  SERIAL_BIT_THERMO_EN = 3,
  SERIAL_BIT_UNUSE0 = 4,  
  SERIAL_BIT_FUSE0_TRIGGER = 5, 
  SERIAL_BIT_FUSE1_TRIGGER = 6,
  SERIAL_BIT_FUSE0_BROKE_TEST = 7,  
  SERIAL_BIT_FUSE1_BROKE_TEST = 8,  
  SERIAL_BIT_TRIPWIRE_TEST = 9,
  SERIAL_BIT_UNUSE1 = 10, 
  SERIAL_BIT_UNUSE2 = 11, 
  SERIAL_BIT_UNUSE3 = 12,  
  SERIAL_BIT_UNUSE4 = 13,
  SERIAL_BIT_UNUSE5 = 14,  
  SERIAL_BIT_UNUSE6 = 15,
};

void serial_hub_initialize(void);
void serial_ctl_out(void);
void serial_set_ctl_bit(unsigned char index, bit what);
bit serial_test_ctl_bit(unsigned char index);
void serial_dump(void);
#endif