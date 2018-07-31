#include <STC89C5xRC.H>
#include <stdio.h>
#include "debug.h"
#include "com.h"
#include "cext.h"

#ifdef     __OSCILLATO_6T__
#define    XTAL 22118400
#else
#define    XTAL 11059200
#endif

#define    baudrate 9600                        // 9600 bps communication baudrate

#define    OLEN  8                              // size of serial transmission buffer
static unsigned   char  ostart;                 // transmission buffer start index
static unsigned   char  oend;                   // transmission buffer end index
static char idata outbuf[OLEN];                 // storage for transmission buffer

#define    ILEN  8                              // size of serial receiving buffer
static unsigned   char  istart;                 // receiving buffer start index
static unsigned   char  iend;                   // receiving buffer end index
static char idata inbuf[ILEN];                  // storage for receiving buffer

bit sendfull;                                   // flag: marks transmit buffer full
bit sendactive;                                 // flag: marks transmitter active

/*--------------------------------------------------------------------------------
 * Serial Interrupt Service Routine
 *------------------------------------------------------------------------------*/
static void com_isr (void) interrupt 4 using 2
{
char c;

  //----- Received data interrupt. -----
  if (RI) {
    c = SBUF;                                   // read character
    RI = 0;                                     // clear interrupt request flag
    if (istart + ILEN != iend) {
      inbuf[iend++ & (ILEN-1)] = c;             // put character into buffer
    }
  }

  //----- Transmitted data interrupt. -----
  if (TI != 0) {
    TI = 0;                                     // clear interrupt request flag
    if (ostart != oend) {                       // if characters in buffer and
      SBUF = outbuf[ostart++ & (OLEN-1)];       // transmit character
      sendfull = 0;                             // clear 'sendfull' flag
    }
    else {                                      // if all characters transmitted
      sendactive = 0;                           // clear 'sendactive'
    }
  }
}


/*--------------------------------------------------------------------------------
 * Function to initialize the serial port and the UART baudrate.
 *------------------------------------------------------------------------------*/
void com_initialize (void) {
  istart = 0;                                  // empty transmit buffers
  iend = 0;
  ostart = 0;                                  // empty transmit buffers
  oend = 0;
  sendactive = 0;                              // transmitter is not active
  sendfull = 0;                                // clear 'sendfull' flag

  SCON = 0x50;                                 // serial port MODE 1, enable serial receiver
                                               // Configure timer 1 as a baud rate generator
  PCON |= 0x80;                                // 0x80=SMOD: set serial baudrate doubler
  TMOD |= 0x20;                                // put timer 1 into MODE 2

  TH1 = (unsigned char) (256 - (XTAL / (16L * 12L * baudrate)));
  TR1 = 1;                                     // start timer 1
  REN = 1;
  ES = 1;                                      // enable serial interrupts
}

//   SCON  = 0xDA;        // SCON: mode 1, 8-bit UART, enable rcvr      */
//   TMOD |= 0x20;        // TMOD: timer 1, mode 2, 8-bit reload        */
//   TH1   = 0xFD;        // TH1:  reload value for 1200 baud @ 12MHz   */
//   TR1   = 1;           // TR1:  timer 1 run                          */
//   TI    = 1;           // TI:   set TI to send first char of UART    */

/*--------------------------------------------------------------------------------
 * putbuf: write a character to SBUF or transmission buffer
 *------------------------------------------------------------------------------*/
void putbuf (char c) {
  if (!sendfull) {                             // transmit only if buffer not full
    if (!sendactive) {                         // if transmitter not active:
      sendactive = 1;                          // transfer first character direct
      SBUF = c;                                // to SBUF to start transmission
    }
    else {
      ES = 0;                                  // disable serial interrupts during buffer update
      outbuf[oend++ & (OLEN-1)] = c;           // put char to transmission buffer
      if (((oend ^ ostart) & (OLEN-1)) == 0) {
         sendfull = 1;
      }                                        // set flag if buffer is full
      ES = 1;                                  // enable serial interrupts again
    }
  }
}


/*--------------------------------------------------------------------------------
 * Replacement routine for the standard library putchar routine.
 * The printf function uses putchar to output a character.
 *------------------------------------------------------------------------------*/
char putchar (char c) {
  if (c == '\n') {                             // expand new line character:
    while (sendfull);                          // wait until there is space in buffer
    putbuf (0x0D);                             // send CR before LF for <new line>
  }
  while (sendfull);                            // wait until there is space in buffer
  putbuf (c);                                  // place character into buffer
  return (c);
}


/*--------------------------------------------------------------------------------
 * Replacement routine for the standard library _getkey routine.
 * The getchar and gets functions uses _getkey to read a character.
 *------------------------------------------------------------------------------*/
char _getkey (void) {
  char c;
  while (iend == istart) {
     ;                                         // wait until there are characters
  }
  ES = 0;                                      // disable serial interrupts during buffer update
  c = inbuf[istart++ & (ILEN-1)];
  ES = 1;                                      // enable serial interrupts again
  return (c);
}

char com_try_get_key(void) 
{
  char c;
  if (iend == istart) {
     return 0;                                         // wait until there are characters
  }
  ES = 0;                                      // disable serial interrupts during buffer update
  c = inbuf[istart++ & (ILEN-1)];
  ES = 1;                                      // enable serial interrupts again
  return (c);
}

void com_enter_powersave(void)
{
  CDBG(("com_enter_powersave\n"));
  REN = 0;
  ES = 0;
}

void com_leave_powersave(void)
{
  REN = 1;
  ES = 1;
  CDBG(("com_leave_powersave\n"));
}
