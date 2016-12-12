#include <STC89C5xRC.H>
#include <stdio.h>

#include "task.h"
#include "debug.h"
#include "cext.h"
#include "led.h"
#include "misc.h"
#include "timer.h"

// interrupt period (1 msec)
#define  TMR_TIME  1e-3 

unsigned char idata counter_1ms;
unsigned char idata counter_25ms;
unsigned char idata counter_250ms;
unsigned char idata counter_1s;

// 每1ms被调用一次
static void timer1_ISR (void) interrupt 5 using 1 
 {
   counter_1ms = (counter_1ms + 1) % 250;
   if((counter_1ms % 7 ) == 0) {
     refresh_led();
   }
   if((counter_1ms % 25) == 0) {
     set_task(EV_SCAN_KEY); 
     counter_25ms = (counter_25ms + 1) % 250;
     if((counter_25ms % 10) == 0) {
       set_task(EV_250MS); 
       counter_250ms = (counter_250ms  + 1) % 250;
       if((counter_250ms % 4) == 0) {
         counter_1s = (counter_1s + 1) % 250;
         set_task(EV_1S); 
       }
     }
   }
   TF2 = 0;
 }

void timer_initialize (float fclk)
{

   int tmr_reload;
  
	 CDBG("timer_initialize %f\n", fclk);  
  
   counter_1ms = 0;  // initialize state counter
	 counter_25ms = 0;
   counter_250ms = 0;
   counter_1s = 0;
  
	// initialization T2CON:
   //   CP/RL2 = 0 (autoreload, no capture),
   //   EXEN2 = 0 (disable external input T2EX),
   //   C/T2 = 0 (timer, no counter),
   //   RCLK = TCLK = 0 (timer, no baud rate generator),
   //   TF2 = EXF2 = 0 (interrupt flags cleared) 
   //   TR2 = 0 (timer stop, please)
   T2CON = 4;

   // Load timer2 autoreload bytes
   tmr_reload = (0xFFFF - (unsigned int)((fclk * TMR_TIME) / 12));
   RCAP2H = high_byte(tmr_reload); 
   RCAP2L = low_byte(tmr_reload);

   // Highest priority for Timer2 interrupts
   PT2 = 1;
   // Enables Timer2 interrupts
   ET2 = 1;
   // Start Timer2
   TR2 = 1;	
}
