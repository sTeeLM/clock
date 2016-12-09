#include <REGX52.H>
#include <string.h>

#include "led.h"
#include "task.h"
#include "misc.h"
#include "timer.h"
#include "debug.h"

char led_data[6];
char led_blink;
char led_index;
unsigned char scan_loop_cnt;

static code char led_code[] =
{
        /* GFEDCBA */
   0x1C, //0011100  o  assume ascii 0!!
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
   0xFF, //         G
   0x09, //0001001  H
   0xFF, //         I
   0x71, //1110001  J  
   0xFF, //         K
   0x47, //1000111  L
   0xFF, //         M
   0x48, //1001000  N
   0x23, //0100011  O
   0x0C, //0001100  P
   0xFF, //         Q
   0xFF, //         R
   0xFF, //         S
   0xFF, //         T
   0x41, //1000001  U
   0xFF, //         V
   0xFF, //         W
   0xFF, //         X
   0x0D, //0001101  Y
   0xFF, //         Z ascii 0x5A
};

void led_initialize (void)
{
  CDBG("led_initialize\n");

  led_blink = 0;
  led_index = 0;
  scan_loop_cnt = 0;
  memset(led_data, 0xFF, sizeof(led_data));
  
  P0 = 0xFF;  
  P2 = 0xFF;

  // for test
  led_set_code(0, '1');
  led_set_code(1, '2');  
  led_set_code(2, '3');
  led_set_code(3, '4');  
  led_set_code(4, '5');
  led_set_code(5, '6');
  
  led_set_dig(0);
  led_set_dig(4);
  
  led_set_blink(0);
  led_set_blink(1);  
}

void led_set_blink(unsigned char i)
{
  if( i < 6 ) {
    led_blink |= 1<<i;
  }    
}

void led_clr_blink(unsigned char i)
{
  if( i < 6 ) {
    led_blink &= ~(1<<i);
  } 
}

void led_clr_dig(unsigned char i)
{
  if( i < 6 ) {
    led_data[i] |= 0x80;
  }
}

void led_set_dig(unsigned char i)
{
  if( i < 6 ) {
    led_data[i] &= ~0x80;
  }
}

void led_set_code(unsigned char i, char c)
{
  if( i < 6 ) {
    led_data[i] &= 0x80;
    if(c == 0) {
      led_data[i] |= led_code[0];
    }
    else if(c <= 'Z' && c >= '-') {
      led_data[i] |= led_code[c - '-' + 1];
    }
  }
}

