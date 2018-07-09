#include "indicator.h"
#include "i2c.h"
#include "serial_hub.h"
#include "debug.h"

void indicator_initialize (void)
{
  CDBG("indicator_initialize\n");
  indicator_clr();
}

void indicator_clr(void)
{
  CDBG("indicator_clr\n");
  serial_set_ctl_bit(SERIAL_BIT_INDICATOR_SW_EN, 0);
  serial_set_ctl_bit(SERIAL_BIT_INDICATOR_RED_EN, 1);  
  serial_set_ctl_bit(SERIAL_BIT_INDICATOR_GREEN_EN, 1);
  serial_ctl_out();
}

void indicator_set(enum indicator_color color, 
  enum indicator_mode mode)
{
  CDBG("indicator_set color = %bu, mode = %bu\n", color, mode);
  switch(color) {
    case INDICATOR_COLOR_RED:
      if(mode == INDICATOR_MODE_BLINK) {
        serial_set_ctl_bit(SERIAL_BIT_INDICATOR_SW_EN, 1);
        serial_set_ctl_bit(SERIAL_BIT_INDICATOR_RED_EN, 0);
      } else if(mode == INDICATOR_MODE_ON) {
        serial_set_ctl_bit(SERIAL_BIT_INDICATOR_SW_EN, 0);
        serial_set_ctl_bit(SERIAL_BIT_INDICATOR_RED_EN, 0);
      } else {
        serial_set_ctl_bit(SERIAL_BIT_INDICATOR_SW_EN, 0);
        serial_set_ctl_bit(SERIAL_BIT_INDICATOR_RED_EN, 1);
      }
      break;
    case INDICATOR_COLOR_GREEN:
      if(mode == INDICATOR_MODE_BLINK) {
        serial_set_ctl_bit(SERIAL_BIT_INDICATOR_SW_EN, 1);
        serial_set_ctl_bit(SERIAL_BIT_INDICATOR_GREEN_EN, 0);
      } else if(mode == INDICATOR_MODE_ON) {
        serial_set_ctl_bit(SERIAL_BIT_INDICATOR_SW_EN, 0);
        serial_set_ctl_bit(SERIAL_BIT_INDICATOR_GREEN_EN, 0);
      } else {
        serial_set_ctl_bit(SERIAL_BIT_INDICATOR_SW_EN, 0);
        serial_set_ctl_bit(SERIAL_BIT_INDICATOR_GREEN_EN, 1);
      }
      break;
  }
  serial_ctl_out();
}
