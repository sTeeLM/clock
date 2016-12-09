#include <REGX52.H>
#include <stdio.h>

#include "task.h"
#include "debug.h"
#include "cext.h"
#include "led.h"
#include "misc.h"

// interrupt period (1 msec)
#define  TMR_TIME  1e-3 

unsigned char counter_1ms;
unsigned char counter_25ms;
unsigned char counter_1s;

static code char led_scan[6] = 
{
  0xFE,
  0xFD,
  0xFB,
  0xF7,
  0xEF,
  0xDF,
};

static void refresh_led(void)
{
  P0 = 0xFF;
  if(led_blink & (1 << led_index)) { // 如果要求闪阿闪
    if(scan_loop_cnt > 15) {   // 不显示数字，只显示点，如果有点的话
      P0 = (led_data[led_index] & 0x80) | 0x7F;
    } else {               //  显示数字和点
      P0 = led_data[led_index];
    }
  } else {                 //  显示数字和点
    P0 = led_data[led_index];
  }
    
  P2 = led_scan[led_index]; // P2某根线供电
  
  led_index ++;
  if(led_index == 6 ) {
    led_index = 0;
    scan_loop_cnt = (scan_loop_cnt + 1) % 32;
  }
}

static void timer1_ISR (void) interrupt 5 using 1
// Timer2 isr 
 { // Clear interrupt 
   TF2 = 0;
   counter_1ms = (counter_1ms + 1) % 250;
   if((counter_1ms % 7 ) == 0) {
     refresh_led();
   }   
   if((counter_1ms % 25) == 0) {
     counter_25ms = (counter_25ms + 1) % 250;
     set_task(EV_SCAN_KEY); 
     if((counter_25ms % 40) == 0) {
       counter_1s = (counter_1s  + 1) % 250;
     }
   }
 }

void timer_initialize (float fclk)
{

   int tmr_reload;
  
	 CDBG("timer_initialize %f\n", fclk);  
  
   counter_1ms = 0;  // initialize state counter
	 counter_25ms = 0;
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
