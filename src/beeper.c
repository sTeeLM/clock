#include <STC89C5xRC.H>
#include "clock.h"
#include "beeper.h"
#include "debug.h"
#include "cext.h"
#include "misc.h"
#include "rom.h"

sbit beeper_out = P1 ^ 0;

static bit beep_enable;

void beeper_initialize (void)
{
  CDBG(("beeper_initialize\n"));
  beep_enable = rom_read(ROM_BEEPER_ENABLE);
  beeper_out  = 1;
}

void beeper_enter_powersave(void)
{
  CDBG(("beeper_enter_powersave\n"));
}

void beeper_leave_powersave(void)
{
  CDBG(("beeper_leave_powersave\n"));
}

bit beeper_get_beep_enable(void)
{
  return beep_enable;
}

void beeper_set_beep_enable(bit enable)
{
  beep_enable = enable;
}

void beeper_write_rom_beeper_enable(void)
{
  rom_write(ROM_BEEPER_ENABLE, beeper_get_beep_enable());
}

void beeper_beep(void)
{ 
  unsigned char c = 30;
  CDBG(("beeper_beep!\n"));
  if(!beep_enable)
    return;
  beeper_out = 1;
  while(c --) {
    beeper_out=~beeper_out;
    delay_10us(2 * 0x13); 
  }
  beeper_out = 1;
}

void beeper_beep_beep_always(void)
{
  unsigned char c = 30;
  CDBG(("beeper_beep_beep_always!\n"));
  beeper_out = 1;
  while(c --) {
    beeper_out=~beeper_out;
    delay_10us(2 * 0x10); 
  }
  
  delay_ms(100);
  
  while(c --) {
    beeper_out=~beeper_out;
    delay_10us(2 * 0x10); 
  }
  beeper_out = 1;
}

void beeper_beep_beep(void)
{
  unsigned char c = 30;
  CDBG(("beeper_beep_beep!\n"));
  if(!beep_enable)
    return;
  beeper_out =0;
  while(c --) {
    beeper_out=~beeper_out;
    delay_10us(2 * 0x10); 
  }
  
  delay_ms(100);
  
  while(c --) {
    beeper_out=~beeper_out;
    delay_10us(2 * 0x10); 
  }
  beeper_out = 1;
}
