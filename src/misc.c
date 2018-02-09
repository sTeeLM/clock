#include <intrins.h>
#include "misc.h"
#include "cext.h"

#ifdef OSCILLATO_6T
// 时钟频率22118400
static void internal_delay_10us(void) 
{
	unsigned char i;

	_nop_();
	i = 6;
	while (--i);
}

static void internal_delay_ms(void)
{
	unsigned char i, j;

	_nop_();
	i = 4;
	j = 146;
	do
	{
		while (--j);
	} while (--i);
}

#else
// 时钟频率11059200
static void internal_delay_10us(void) 
{
	unsigned char i;

	i = 2;
	while (--i);
}

static void internal_delay_ms(void)
{
	unsigned char i, j;
	_nop_();
	i = 2;
	j = 199;
	do
	{
		while (--j);
	} while (--i);
}
#endif


void delay_10us(unsigned char t)
{
  while(t --) {
    internal_delay_10us();
  }
}

void delay_ms(unsigned char t) 
{     
  while(t--) {      
    internal_delay_ms();
  } 
}
