#include <intrins.h>
#include "misc.h"

void delay_us(unsigned char c)
{
  while( c --) {
    _nop_();
  }
}

void delay_ms(unsigned char c)
{
  unsigned char i;
  while( c --) {
    for (i = 0 ; i < 200; i++) {
      _nop_();
    }
    for (i = 0 ; i < 200; i++) {
      _nop_();
    }
    for (i = 0 ; i < 200; i++) {
      _nop_();
    }
    for (i = 0 ; i < 200; i++) {
      _nop_();
    }
  }
}