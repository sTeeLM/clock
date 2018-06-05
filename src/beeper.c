#include <STC89C5xRC.H>
#include "clock.h"
#include "beeper.h"
#include "debug.h"
#include "cext.h"
#include "misc.h"
#include "rom.h"

#define MAX_BEEPER_MUSIC_TO   70 // 秒
#define BEEPER_MUSIC_TO_STEP  10 // 秒
#define MIN_BEEPER_MUSIC_TO   10 // 秒

// 格式为：频率常数，节拍常数，频率常数，节拍常数
// 频率常数：n为多少个20us延迟
// 节拍常数：m为多少个10ms延迟（10ms = 500个20us延迟）

// 八月桂花香
static unsigned char code music_table0[]={
  0x18,0x30,0x1c,0x10,0x20,0x40,0x1c,0x10,0x18,0x10,
  0x20,0x10,0x1c,0x10,0x18,0x40,0x1c,0x20,0x20,0x20,
  0x1c,0x20,0x18,0x20,0x20,0x80,0xff,0xFF,0x20,0x30,
  0x1c,0x10,0x18,0x20,0x15,0x20,0x1c,0x20,0x20,0x20,
  0x26,0x40,0x20,0x20,0x2b,0x20,0x26,0x20,0x20,0x20,
  0x30,0x80,0xff,0xFF,0x20,0x20,0x1c,0x10,0x18,0x10,
  0x20,0x20,0x26,0x20,0x2b,0x20,0x30,0x20,0x2b,0x40,
  0x20,0x20,0x1c,0x10,0x18,0x10,0x20,0x20,0x26,0x20,
  0x2b,0x20,0x30,0x20,0x2b,0x40,0x20,0x30,0x1c,0x10,
  0x18,0x20,0x15,0x20,0x1c,0x20,0x20,0x20,0x26,0x40,
  0x20,0x20,0x2b,0x20,0x26,0x20,0x20,0x20,0x30,0x80,
  0x20,0x30,0x1c,0x10,0x20,0x10,0x1c,0x10,0x20,0x20,
  0x26,0x20,0x2b,0x20,0x30,0x20,0x2b,0x40,0x20,0x15,
  0x1f,0x05,0x20,0x10,0x1c,0x10,0x20,0x20,0x26,0x20,
  0x2b,0x20,0x30,0x20,0x2b,0x40,0x20,0x30,0x1c,0x10,
  0x18,0x20,0x15,0x20,0x1c,0x20,0x20,0x20,0x26,0x40,
  0x20,0x20,0x2b,0x20,0x26,0x20,0x20,0x20,0x30,0x30,
  0x20,0x30,0x1c,0x10,0x18,0x40,0x1c,0x20,0x20,0x20,
  0x26,0x40,0x13,0x60,0x18,0x20,0x15,0x40,0x13,0x40,
  0x18,0x80,0x00,0x00};  

//祝你平安
static unsigned char code music_table1[]=
{
  0x26,0x20,0x20,0x20,0x20,0x20,0x26,0x10,0x20,0x10,
  0x20,0x80,0x26,0x20,0x30,0x20,0x30,0x20,0x39,0x10,
  0x30,0x10,0x30,0x80,0x26,0x20,0x20,0x20,0x20,0x20,
  0x1c,0x20,0x20,0x80,0x2b,0x20,0x26,0x20,0x20,0x20,
  0x2b,0x10,0x26,0x10,0x2b,0x80,0x26,0x20,0x30,0x20,
  0x30,0x20,0x39,0x10,0x26,0x10,0x26,0x60,0x40,0x10,
  0x39,0x10,0x26,0x20,0x30,0x20,0x30,0x20,0x39,0x10,
  0x26,0x10,0x26,0x80,0x26,0x20,0x2b,0x10,0x2b,0x10,
  0x2b,0x20,0x30,0x10,0x39,0x10,0x26,0x10,0x2b,0x10,
  0x2b,0x20,0x2b,0x40,0x40,0x20,0x20,0x10,0x20,0x10,
  0x2b,0x10,0x26,0x30,0x30,0x80,0x18,0x20,0x18,0x20,
  0x26,0x20,0x20,0x20,0x20,0x40,0x26,0x20,0x2b,0x20,
  0x30,0x20,0x30,0x20,0x1c,0x20,0x20,0x20,0x20,0x80,
  0x1c,0x20,0x1c,0x20,0x1c,0x20,0x30,0x20,0x30,0x60,
  0x39,0x10,0x30,0x10,0x20,0x20,0x2b,0x10,0x26,0x10,
  0x2b,0x10,0x26,0x10,0x26,0x10,0x2b,0x10,0x2b,0x80,
  0x18,0x20,0x18,0x20,0x26,0x20,0x20,0x20,0x20,0x60,
  0x26,0x10,0x2b,0x20,0x30,0x20,0x30,0x20,0x1c,0x20,
  0x20,0x20,0x20,0x80,0x26,0x20,0x30,0x10,0x30,0x10,
  0x30,0x20,0x39,0x20,0x26,0x10,0x2b,0x10,0x2b,0x20,
  0x2b,0x40,0x40,0x10,0x40,0x10,0x20,0x10,0x20,0x10,
  0x2b,0x10,0x26,0x30,0x30,0x80,0x00,0x00};  

//送别 
static unsigned char code music_table2[]={
  0x20,0x40,0x26,0x20,0x20,0x20,0x17,0x80,0x1c,0x40,
  0x17,0x40,0x20,0x80,0x20,0x40,0x2f,0x20,0x2a,0x20,
  0x26,0x40,0x2a,0x20,0x2f,0x20,0x2a,0x80,0xff,0xff,
  0x20,0x40,0x26,0x20,0x20,0x20,0x17,0x60,0x19,0x20,
  0x1c,0x40,0x17,0x40,0x20,0x80,0x20,0x40,0x2a,0x20,
  0x26,0x20,0x24,0x60,0x32,0x20,0x2f,0x80,0xff,0xff,
  0x1c,0x40,0x17,0x40,0x17,0x80,0x19,0x40,0x1c,0x20,
  0x19,0x20,0x17,0x80,0x1c,0x20,0x19,0x20,0x17,0x20,
  0x1c,0x20,0x1c,0x20,0x20,0x20,0x26,0x20,0x2f,0x20,
  0x2a,0x80,0xff,0xff,0x20,0x40,0x26,0x20,0x20,0x20,
  0x17,0x60,0x19,0x20,0x1c,0x40,0x17,0x40,0x20,0x80,
  0x20,0x40,0x2a,0x20,0x26,0x20,0x24,0x60,0x32,0x20,
  0x2f,0x80,0xff,0xff,0x20,0x40,0x26,0x20,0x20,0x20,
  0x17,0x80,0x1c,0x40,0x17,0x40,0x20,0x80,0x20,0x40,
  0x2f,0x20,0x2a,0x20,0x26,0x40,0x2a,0x20,0x2f,0x20,
  0x2a,0x80,0xff,0xff,0x20,0x40,0x26,0x20,0x20,0x20,
  0x17,0x60,0x19,0x20,0x1c,0x40,0x17,0x40,0x20,0x80,
  0x20,0x40,0x2a,0x20,0x26,0x20,0x24,0x60,0x32,0x20,
  0x2f,0x80,0x00,0x00}; 


// 两只老虎
static unsigned char code music_table3[]={
  0x2f,0x40,0x2a,0x40,0x26,0x40,0x2f,0x40,
  0x2f,0x40,0x2a,0x40,0x26,0x40,0x2f,0x40,
  0x26,0x40,0x24,0x40,0x20,0x60,
  0x26,0x40,0x24,0x40,0x20,0x60,
  0x20,0x20,0x1c,0x40,0x20,0x40,0x24,0x20,0x26,0x40,0x2f,0x40,
  0x20,0x20,0x1c,0x40,0x20,0x40,0x24,0x20,0x26,0x40,0x2f,0x40,
  0x2f,0x40,0x20,0x40,0x2f,0x40,0xff,0xff,
  0x2f,0x40,0x20,0x40,0x2f,0x40,0x00,0x00};

static unsigned char * code music_table[] = {
  music_table0,
  music_table1,
  music_table2,
  music_table3
};

sbit beeper_out = P1 ^ 0;

static bit idata beeper_stop;
static bit beep_enable;
static unsigned char pai;
static unsigned char beeper_music_index;
static unsigned char beeper_music_to;

// 每10ms 调用一次
static void beeper_ISR (void) interrupt 5 using 2 
{
  //all_music[beeper_music_index]();
  if(pai > 0)
    pai --;
  TF2 = 0;
}


void beeper_initialize (void)
{
  CDBG("beeper_initialize\n");
   //   CP/RL2 = 0 (autoreload, no capture),
   //   EXEN2 = 0 (disable external input T2EX),
   //   C/T2 = 0 (clock, no counter),
   //   RCLK = TCLK = 0 (clock, no baud rate generator),
   //   TF2 = EXF2 = 0 (interrupt flags cleared) 
   //   TR2 = 0 (clock stop, please)
   T2CON = 0;

   // Load clock2 autoreload bytes  
#ifdef __OSCILLATO_6T__
   TH2 = (0xFFFF-10000)/128;
   TL2 = (0xFFFF-10000)%128;
   RCAP2H = (0xFFFF-10000)/128;
   RCAP2L = (0xFFFF-10000)%128;
#else
   TH2 = (0xFFFF-10000)/256;
   TL2 = (0xFFFF-10000)%256;
   RCAP2H = (0xFFFF-10000)/256;
   RCAP2L = (0xFFFF-10000)%256;
#endif
  
   // Normal priority for Timer2 interrupts
   PT2 = 0;
   // Disable Timer2 interrupts
   ET2 = 0;
   // Start Timer2
   TR2 = 0;  
   
   beeper_music_index = rom_read(ROM_BEEPER_MUSIC_INDEX) % BEEPER_MUSIC_CNT;
   beeper_stop = 1;
   beeper_music_to = rom_read(ROM_BEEPER_MUSIC_TO);
   beep_enable = rom_read(ROM_BEEPER_ENABLE);
   beeper_out  = 1;
}

void beeper_enter_powersave(void)
{
  CDBG("beeper_enter_powersave\n");
}

void beeper_leave_powersave(void)
{
  CDBG("beeper_leave_powersave\n");
}

bit beeper_get_beep_enable(void)
{
  return beep_enable;
}

void beeper_set_beep_enable(bit enable)
{
  beep_enable = enable;
}

unsigned char beeper_get_music_to(void)
{
  return beeper_music_to;
}

void beeper_inc_music_to(void)
{
  beeper_music_to = (beeper_music_to + BEEPER_MUSIC_TO_STEP) % MAX_BEEPER_MUSIC_TO;
  if(beeper_music_to == 0)
    beeper_music_to = MIN_BEEPER_MUSIC_TO;
}

void beeper_inc_music_index(void)
{
  beeper_music_index = (++beeper_music_index) % BEEPER_MUSIC_CNT;
  CDBG("beeper_inc_music_index %bu\n", beeper_music_index);
}

enum beeper_music beeper_get_music_index()
{
  return beeper_music_index;
}

static void _beeper_stop_play(void)
{
  TR2 = 0;
  ET2 = 0;
  pai = 0;
  beeper_stop = 1;
  beeper_out = 1;
}

static bit _beepler_play(unsigned char * music, bit once)
{
  unsigned char cm;
  unsigned char tune;
  unsigned char start_s;

  pai  = 0;  
  cm = 0;
  tune = 0;
  beeper_stop = 0;
  start_s = clock_get_sec_256();
  
  CDBG("_beepler_play: beeper_music_to = %bu s\n", beeper_music_to);
  
  while (!beeper_stop)     
  {
    switch (music[cm]) { 
      case 0x00:  
        pai  = 0;  
        cm = 0;
        tune = 0;
        if(once) {
          _beeper_stop_play();
          return 0;
        }
        // 直到一首歌播放完毕才检查超时
        if(time_diff_now(start_s) >= beeper_music_to) {
          _beeper_stop_play();
          return 0;
        }
        break;          
      case 0xff: 
        if(time_diff_now(start_s) >= beeper_music_to) {
          _beeper_stop_play();
          return 0;
        } 
        cm += 2;
        delay_ms(250);
        break;          
      default:   
        tune=music[cm++];             
        pai=music[cm++];             
        beeper_out = 1;            
        TR2 = 1;
        ET2 = 1;
        while (pai!=0 && !beeper_stop) { 
          beeper_out=~beeper_out; 
          delay_10us(2 * tune); 
        }
        beeper_out = 1; 
    }
    if((cm % 10) == 0) {
      CDBG("MUSIC TO CHECK: %bu\n", time_diff_now(start_s));
      if(time_diff_now(start_s) >= beeper_music_to) {
        _beeper_stop_play();
        return 0;
      } 
    }
  }
  _beeper_stop_play();
  return 1;
}

void beeper_beep(void)
{ 
  unsigned char c = 30;
  CDBG("beeper_beep!\n");
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
  CDBG("beeper_beep_beep_always!\n");
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
  CDBG("beeper_beep_beep!\n");
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

bit beeper_play_music(void)
{

  CDBG("beeper_play_music\n");
  return _beepler_play(music_table[beeper_music_index], 0);
}

void beeper_stop_music(void)
{
  _beeper_stop_play();
}
