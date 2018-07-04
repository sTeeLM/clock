#include "indicator.h"
#include "i2c.h"
#include "debug.h"


#define INDICATOR_I2C_ADDRESS 0xC4//11000100

void indicator_initialize (void)
{
  unsigned char val;
  CDBG("indicator_initialize\n");
  // Mode register 1
  // 00001111
  // does not respond to sub-call, all-call
  // Normal mode, dont sleep
  val = 0xF;
  I2C_Put(INDICATOR_I2C_ADDRESS, 0x0, val);
  
  // Mode register 2
  // 00100001
  // Group control = blinking, DMBLNK = 1
  // Output logic state not inverted
  // Outputs change on STOP command
  // The 4 LED outputs are configured with an open-drain structure
  val = 0x21;
  I2C_Put(INDICATOR_I2C_ADDRESS, 0x1, val);
  
  // PWM registers 0 to 3, Individual brightness control registers
  val = 0xFF;
  I2C_Put(INDICATOR_I2C_ADDRESS, 0x2, val);
  I2C_Put(INDICATOR_I2C_ADDRESS, 0x3, val);
  I2C_Put(INDICATOR_I2C_ADDRESS, 0x4, val);
  I2C_Put(INDICATOR_I2C_ADDRESS, 0x5, val);
  
  //Group duty cycle control, GRPPWM
  val = 0x3F;
  I2C_Put(INDICATOR_I2C_ADDRESS, 0x6, val);
  
  
  // Group frequency, GRPFREQ
  val = 0xFF; // 10.73 s
  I2C_Put(INDICATOR_I2C_ADDRESS, 0x7, val);
  
  // LED driver output state, LEDOUT
  val = 0x0;
  I2C_Put(INDICATOR_I2C_ADDRESS, 0x8, val); 
}

void indicator_clr(void)
{
  unsigned char val;
  CDBG("indicator_clr\n");
  val = 0x0;
  I2C_Put(INDICATOR_I2C_ADDRESS, 0x8, val);
}

void indicator_set(enum indicator_color color, 
  enum indicator_mode mode)
{
  unsigned char val, tmp;
  CDBG("indicator_set, color = %bu, mode = %bu\n", color, mode);
  val = 0;
  tmp = 0;
  switch(mode){
    case INDICATOR_MODE_BLINK: 
      tmp = 3;
    break;
    case INDICATOR_MODE_OFF: 
      tmp = 0;
    break;
    case INDICATOR_MODE_ON: 
      tmp = 1;
    break;
  }
  I2C_Get(INDICATOR_I2C_ADDRESS, 0x8, &val);
  switch(color) {
    case INDICATOR_COLOR_RED:
      val &= ~0x3;
      val |= tmp; 
    break;
    case INDICATOR_COLOR_GREEN:
      val &= ~0xC;
      val |= tmp << 2; 
    break;
    case INDICATOR_COLOR_BLUE: 
      val &= ~0x30;
      val |= tmp << 4; 
    break;      
  }
  I2C_Put(INDICATOR_I2C_ADDRESS, 0x8, val);
}
