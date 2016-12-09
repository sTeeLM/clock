#include <REGX52.H>


#include "debug.h"
#include "i2c.h"

sbit RTC_RESET = P1 ^ 5;

#define RTC_RESET_PULSE_DELAY 20
#define RTC_I2C_ADDRESS  0xD0 //11010000

static void rtc_ISR (void) interrupt 2 using 1
{
  
}

static void dump_rtc(void)
{
	unsigned char addr;
	unsigned char c;
	// dump rtc contents
	for(addr = 0; addr < 0xff; addr ++) {
		I2C_Get(RTC_I2C_ADDRESS, addr, &c);
		CDBG("%bx = %bx\n", addr,  c);
	}
  CDBG("%bx = %bx\n", 0xff,  c);
}

void rtc_initialize (void)
{
	unsigned int count = RTC_RESET_PULSE_DELAY;
	
	CDBG("rtc_initialize\n");
	
	I2C_Init();

  // reset rtc
	RTC_RESET = 0;
	while(count --);
	RTC_RESET = 1;	

	// dump rtc contents
	//dump_rtc();
}