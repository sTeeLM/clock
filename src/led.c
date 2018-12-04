#include <STC89C5xRC.H>
#include <string.h>

#include "led.h"
#include "task.h"
#include "misc.h"
#include "clock.h"
#include "debug.h"

unsigned char idata led_data[6];
unsigned char idata led_blink;
static unsigned char idata led_index;
static unsigned char idata scan_loop_cnt;

unsigned char saved_led_data[6];
unsigned char saved_led_blink;
static unsigned char saved_led_index;
static unsigned char saved_scan_loop_cnt;

bit led_powersave;

/*        A
        F   B
          G
        E   C  
          D
*/

unsigned char code led_code[] =
{
        /* GFEDCBA */
   0x1C, //0011100  o  degree Celsius assume ascii 0!!
   0x3F, //0111111  -  ascii: 0x2D
   0xFF, //         .
   0xFF, //         /
   0x40, //1000000  0    
   0x79, //1111001  1
   0x24, //0100100  2
   0x30, //0110000  3
   0x19, //0011001  4
   0x12, //0010010  5
   0x02, //0000010  6
   0x78, //1111000  7
   0x00, //0000000  8
   0x10, //0010000  9
   0xFF, //         :
   0xFF, //         ;
   0x27, //0100111  <
   0xFF, //         =
   0x33, //0110011  >
   0xFF, //         ?
   0xFF, //         @  
   0x08, //0001000  A
   0x03, //0000011  B
   0x46, //1000110  C
   0x21, //0100001  D
   0x06, //0000110  E
   0x0E, //0001110  F
   0x10, //0010000  G == 9
   0x09, //0001001  H
   0x4F, //1001111  I
   0x71, //1110001  J 
   0xFF, //         K
   0x47, //1000111  L
   0xFF, //         M
   0x48, //1001000  N
   0x23, //0100011  O
   0x0C, //0001100  P
   0xFF, //         Q
   0x2F, //0101111  R
   0x12, //0010010  S
   0x07, //0000111  T
   0x41, //1000001  U
   0xFF, //         V
   0xFF, //         W
   0xFF, //         X
   0x0D, //0001101  Y
   0xFF, //         Z ascii 0x5A
};

static code char led_scan[6] = 
{
  0xFE,
  0xFD,
  0xFB,
  0xF7,
  0xEF,
  0xDF,
};

#pragma NOAREGS

// 在时钟中断中调用！
void refresh_led(void)
{
  P0 = 0xFF;
  P2 = 0xFF;  
  if(led_powersave) return;
  
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
#pragma AREGS 

void led_initialize (void)
{
  unsigned char i, j;

  led_powersave = 0;
  led_blink = 0;
  led_index = 0;
  scan_loop_cnt = 0;
  memset(led_data, 0xFF, sizeof(led_data));
  
  P0 = 0xFF; 
  P2 = 0xFF;  

  for(i = 0; i < sizeof(led_code); i ++) {
    led_data[0] = led_code[i];
    led_data[1] = led_code[i];
    led_data[2] = led_code[i];
    led_data[3] = led_code[i];
    led_data[4] = led_code[i];
    led_data[5] = led_code[i];  
    for(j = 0; j < 6 ; j ++) {
      refresh_led();
      delay_ms(3);
    }
  }
  
  led_blink = 0;
  led_index = 0;
  scan_loop_cnt = 0;
  memset(led_data, 0x3F|0x80, sizeof(led_data));
  
  P0 = 0xFF; 
  P2 = 0xFF;  
}

void led_clear(void)
{
  led_blink = 0;
  memset(led_data, 0xFF, sizeof(led_data));
}

void led_set_blink(unsigned char i)
{
  if( i < 6 ) {
    led_blink |= 1<<i;
  }    
}

void led_clr_all_blink(void)
{
  led_blink = 0;
}

void led_clr_blink(unsigned char i)
{
  if( i < 6 ) {
    led_blink &= ~(1<<i);
  } 
}

void led_clr_dp(unsigned char i)
{
  if( i < 6 ) {
    led_data[i] |= 0x80;
  }
}

void led_set_dp(unsigned char i)
{
  if( i < 6 ) {
    led_data[i] &= ~0x80;
  }
}


// c为ascii码，0为温度的‘度’，255为全黑
void led_set_code(unsigned char i, char c)
{
  if( i < 6 ) {
    led_data[i] &= 0x80;
    if(c == 0) {
      led_data[i] |= led_code[0];
    }
    else if(c <= 'Z' && c >= '-') {
      led_data[i] |= led_code[c - '-' + 1];
    } else {
      led_data[i] |= 0x7F;
    }
  }
}

void led_enter_powersave(void)
{
  led_powersave = 1;
  
  P0 = 0xFF;
  P2 = 0xFF;
  led_clear();
}

void led_leave_powersave(void)
{

  P0 = 0x0;
  P2 = 0xFF;
  led_clear();
  
  led_powersave = 0;
}

void led_save(void)
{
  memcpy(saved_led_data, led_data, sizeof(led_data)); 
  saved_led_blink = led_blink;
  saved_led_index = led_index;
  saved_scan_loop_cnt = scan_loop_cnt;
}

void led_restore(void)
{
  memcpy(led_data, saved_led_data, sizeof(led_data)); 
  led_blink = saved_led_blink;
  led_index = saved_led_index;
  scan_loop_cnt = saved_scan_loop_cnt;
}
