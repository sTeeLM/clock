#include <string.h>
#include "radio.h"
#include "debug.h"
#include "rom.h"
#include "i2c.h"
#include "serial_hub.h"



static unsigned char data_out[5]; // 待写入
static unsigned char data_in[5];  // 待读出

static unsigned char volume;
static unsigned int freqency;

static bit radio_enabled;

#define RADIO_I2C_ADDR   0xC0
#define RADIO_VOLUME_I2C_ADDR 0x58 //01011000
                       
#define RADIO_MAX_FREQ 1085 // 108.5MHz
#define RADIO_MIN_FREQ 700  // 70.0MHz

#define RADIO_MAX_VOLUME 100
#define RADIO_MIN_VOLUME 0  // VOLUME == 0 is MUTE

// 这些函数用来修改radio_data，不对外暴露
static void _radio_read_data(void) // 读出数据到data_in
{
  unsigned char i;
#ifndef __CLOCK_EMULATE__  
  I2C_Start();
  I2C_Write(RADIO_I2C_ADDR | 0x1);
  if(I2C_GetAck()) {
    I2C_Stop();
  } else {
    for(i = 0 ; i < 5; i ++) {
      data_in[i] = I2C_Read();
      I2C_PutAck(i == 4 ? 1 : 0);
    }
    I2C_Stop();
  }
#else
  i = 0;
#endif
  CDBG(("_radio_read_data %02bx|%02bx|%02bx|%02bx|%02bx\n", data_in[0], data_in[1],data_in[2],data_in[3],data_in[4]));
}

static void _radio_set_bit(unsigned char * buf, unsigned char byte, unsigned char n, bit val)
{
  unsigned char tmp;
  
  tmp = 1;
  buf[byte] &= ~(tmp << n);
  
  if(val) {
    tmp = val;
    buf[byte] |= (tmp << n);
  }
}

static bit _radio_get_bit(unsigned char * buf, unsigned char byte, unsigned char n)
{
  return (buf[byte] & (1 << n)) != 0;
}

static void _radio_write_data(void)// data_out数据写入芯片
{
  unsigned char i;
  CDBG(("_radio_write_data %02bx|%02bx|%02bx|%02bx|%02bx\n", data_out[0], data_out[1],data_out[2],data_out[3],data_out[4]));
#ifndef __CLOCK_EMULATE__ 
  I2C_Start();
  I2C_Write(RADIO_I2C_ADDR);
  if(I2C_GetAck()) {
    I2C_Stop();
  } else {
    for( i = 0 ; i < 5; i ++) {
      I2C_Write(data_out[i]);
      if(I2C_GetAck()) {
        break;
      }
    }
    I2C_Stop();
  }
#else
  i = 0;
#endif
}

// formula for HIGH side LO injection:
// N = 4 * (fRF + fIF) / fref

// formula for LOW side LO injection:
// N = 4 * (fRF - fIF) / fref

// N: decimal value of PLL word
// fRF = the wanted tuning frequency [Hz]
// fIF = the intermediate frequency [Hz] = 225 kHz
// fref = the reference frequency [Hz] = 32.768 kHz for the 32.768 kHz crystal; 
// fref = 50 kHz for the 13 MHz crystal or when externally clocked with 6.5 MHz.

static void _radio_set_pll(unsigned char * buf, unsigned int freq)
{
  unsigned long tmp;

  if(_radio_get_bit(data_out, 2, 4)) {
    tmp = freq * 100000 + 225000;
  } else {
    tmp = freq * 100000 - 225000;
  }
  
  tmp = tmp * 4 / 32768;
  
  CDBG(("_radio_set_pll: freq = %u-> PLL word = 0x%08Lx\n", freq, tmp));
  
  buf[1] = (unsigned char)(tmp & 0xFF);
  buf[0] &= 0xC0;
  buf[0] |= (unsigned char)((tmp >> 8) & 0x3F);
}

static unsigned int _radio_get_pll(unsigned char * buf)
{
  unsigned long tmp;
  
  tmp = (buf[0] & 0x3F) * 256 + buf[1];
  
  CDBG(("_radio_get_pll: PLL word = 0x%08Lx", tmp));
  
  tmp = tmp * 32768 / 4;
  
  if(_radio_get_bit(data_out, 2, 4)) {
    tmp = (tmp - 225000) / 100000;
  } else {
    tmp = (tmp + 225000) / 100000;
  }
  
  CDBG(("-> freq = %u\n", (unsigned int) (tmp & 0xFFFF)));
  
  return (unsigned int) (tmp & 0xFFFF);
}

static void radio_set_power(bit val)
{
  serial_set_ctl_bit(SERIAL_BIT_RADIO_EN, val);
  serial_ctl_out();
}

static void radio_set_pa_mute(bit val)
{ 
  serial_set_ctl_bit(SERIAL_BIT_RADIO_MUTE, val);
  serial_ctl_out();
}

static void radio_set_pa_sd(bit val)
{ 
  serial_set_ctl_bit(SERIAL_BIT_RADIO_SD, !val);
  serial_ctl_out();
}

static void radio_set_pa_volume(unsigned char val)
{
  unsigned int tmp;
  tmp = val / 100 * 255;
#ifndef __CLOCK_EMULATE__  
  I2C_Put(RADIO_VOLUME_I2C_ADDR, 0, (unsigned char)tmp);
#endif
}

static void radio_load_rom(void)
{
  unsigned int lval;
  bit sval; 

  memset(data_out, 0, sizeof(data_out));
  
  volume = rom_read(ROM_RADIO_VOLUME);
  
  _radio_set_bit(data_out, 0, 7, volume == 0 ? 1: 0); // MUTE
  _radio_set_bit(data_out, 0, 6, 0); // SM
  lval = rom_read(ROM_RADIO_FREQ_HI);
  lval = (lval << 8) & 0xFF00;
  lval |= rom_read(ROM_RADIO_FREQ_LO);
  freqency = lval;
  if(freqency < RADIO_MIN_FREQ) freqency = RADIO_MIN_FREQ;
  if(freqency > RADIO_MAX_FREQ) freqency = RADIO_MAX_FREQ;
  _radio_set_pll(data_out, freqency);
  _radio_set_bit(data_out, 2, 7, 0); // SUD
  _radio_set_bit(data_out, 2, 6, 0); // SSL
  _radio_set_bit(data_out, 2, 5, 0); // SSL
  sval = rom_read(ROM_RADIO_HLSI);
  _radio_set_bit(data_out, 2, 4, sval);
  sval   = rom_read(ROM_RADIO_MS);
  _radio_set_bit(data_out, 2, 3, sval);
  _radio_set_bit(data_out, 2, 2, 0); // MR
  _radio_set_bit(data_out, 2, 1, 0); // ML
  _radio_set_bit(data_out, 2, 0, 0); // SWP1
  _radio_set_bit(data_out, 3, 7, 0); // SWP2
  _radio_set_bit(data_out, 3, 6, 0); // STBY
  sval   = rom_read(ROM_RADIO_BL);
  _radio_set_bit(data_out, 3, 5, sval);
  _radio_set_bit(data_out, 3, 4, 1); // XTAL
  _radio_set_bit(data_out, 3, 3, 0);// SMUTE
  sval  = rom_read(ROM_RADIO_HCC);
  _radio_set_bit(data_out, 3, 2, sval);
  sval  = rom_read(ROM_RADIO_SNC);
  _radio_set_bit(data_out, 3, 1, sval);
  _radio_set_bit(data_out, 3, 0, 0); // SI
  _radio_set_bit(data_out, 4, 7, 0); // PLLREF
  sval  = rom_read(ROM_RADIO_DTC);
  _radio_set_bit(data_out, 4, 6, sval);
  _radio_write_data();
  
  _radio_read_data(); // 填充data_in
  
  radio_set_pa_volume(volume); 
}

void radio_initialize (void)
{
  CDBG(("radio_initialize\n"));
  radio_set_pa_mute(1);
  radio_set_pa_sd(1);
  radio_set_power(1);
  radio_enabled = 0;
}

void radio_enter_powersave(void)
{

}

void radio_leave_powersave(void)
{
  
}

void radio_enable(bit enable)
{
  CDBG(("radio_enable %bu\n", enable ? 1 : 0));
  if(!radio_enabled && enable) {
    radio_set_pa_mute(1);
    radio_set_pa_sd(1);
    radio_set_power(0);
    radio_load_rom();
    radio_set_pa_sd(0);
    radio_set_pa_mute(0);
    radio_enabled = 1;
  } else if(radio_enabled && !enable){
    radio_set_pa_mute(1);
    radio_set_pa_sd(1);
    radio_set_power(1);
    radio_enabled = 0;
  }
}

static unsigned int radio_search_station(bit prev, RADIO_CB_PROC cb)
{
  bit found = 0;
#ifdef __CLOCK_EMULATE__ 
  unsigned char cnt;
  cnt = 0;
#endif
  
  while( (!prev && freqency < RADIO_MAX_FREQ) || (prev && freqency > RADIO_MIN_FREQ) ) {
    prev ? freqency -- : freqency ++;
    radio_set_frequency(freqency);
    if(cb != NULL) {
      cb(freqency);
    }
    if(((data_in[3] & 0xF0 ) >> 4) >= 5) {
      CDBG(("radio_%s_station: found %u\n", prev ? "prev" : "next", freqency));
      found = 1;
      break;
    }
#ifdef __CLOCK_EMULATE__ 
    if( ++ cnt >= 23) {
      found = 1;
      break;
    }
#endif    
  }
  
  if(!found) {
    CDBG(("radio_%s_station: not found!\n", prev ? "prev" : "next"));
  }    
  
  return freqency;
}

unsigned int radio_prev_station(RADIO_CB_PROC cb)
{
  return radio_search_station(1, cb);
}

unsigned int radio_next_station(RADIO_CB_PROC cb)
{
  return radio_search_station(0, cb);
}

void radio_set_frequency(unsigned int freq)
{
  unsigned int out_freq;
  if(freq >= RADIO_MAX_FREQ || freq <= RADIO_MIN_FREQ) 
    return;
  freqency = freq;
  _radio_set_pll(data_out, freq);
  _radio_write_data();
  _radio_read_data();
  out_freq = _radio_get_pll(data_in);
  CDBG(("freqency write: %u\n", freqency));
  CDBG(("   freqency read: %u\n", out_freq));
  CDBG(("   RF Adc Level: %u\n" ,((data_in[3] & 0xF0 ) >> 4)));
  CDBG(("   IF Level: 0x%02bx\n" ,(data_in[2] & 0x7F)));
  CDBG(("   Stereo: %s\n",((data_in[2] & 0x80) != 0) ? "ON" : "OFF")); 
  return;
}

static unsigned int radio_change_frequency(bit dec)
{
  if(dec && freqency <= RADIO_MIN_FREQ || !dec && freqency >= RADIO_MAX_FREQ) 
    return freqency;
  
  dec ? freqency -- : freqency ++;
  radio_set_frequency(freqency);
   
  return freqency;
}

bit radio_get_stereo(void)
{
  return _radio_get_bit(data_in, 2, 7);
}

unsigned int radio_dec_frequency(void)
{
  return radio_change_frequency(1);
}

unsigned int radio_inc_frequency(void)
{
  return radio_change_frequency(0);
}

unsigned int radio_get_frequency(void)
{
  return freqency;
}

void radio_write_rom_frequency(void)
{
  rom_write(ROM_RADIO_FREQ_HI, (unsigned char)((freqency & 0xFF00) >> 8));
  rom_write(ROM_RADIO_FREQ_LO, (unsigned char)(freqency & 0xFF));
}

unsigned char radio_inc_volume(void)
{
  if(volume >= RADIO_MAX_VOLUME)
    return volume;
  
  volume ++;
  radio_set_volume(volume);
  return volume;
}

unsigned char radio_dec_volume(void)
{
  if(volume <= RADIO_MIN_VOLUME)
    return volume;
  
  volume --;
  radio_set_volume(volume);
  return volume;
}

unsigned char radio_get_volume(void)
{
  return volume;
}

unsigned char radio_set_volume(unsigned char val)
{
  if(val > 100)
    val = 100;
  
  if(volume == 0 && val != 0) {
    _radio_set_bit(data_out, 0, 7, 0);
    _radio_write_data();
  } else if(volume != 0 && val == 0) {
    _radio_set_bit(data_out, 0, 7, 1);
    _radio_write_data();
  }
  
  volume = val;
  
  radio_set_pa_volume(volume);
  
  return volume;
}

void radio_write_rom_volume(void)
{
  rom_write(ROM_RADIO_VOLUME, volume);
}

bit radio_get_hlsi(void)
{
  return _radio_get_bit(data_out, 2, 4);
}

bit radio_set_hlsi(bit val)
{
  _radio_set_bit(data_out, 2, 4, val);
  _radio_set_pll(data_out, freqency);
  _radio_write_data();
  _radio_read_data(); // 填充data_in
  return val;
}

void radio_write_rom_hlsi(void)
{
  rom_write(ROM_RADIO_HLSI, radio_get_hlsi());
}

bit radio_get_ms(void)
{
  return _radio_get_bit(data_out, 2, 3);
}

bit radio_set_ms(bit val)
{
  _radio_set_bit(data_out, 2, 3, val);
  _radio_write_data();
  _radio_read_data(); // 填充data_in
  return val;
}

void radio_write_rom_ms(void)
{
  rom_write(ROM_RADIO_MS, radio_get_ms());
}

enum radio_bl_level radio_get_bl(void)
{
  return _radio_get_bit(data_out, 3, 5);
}

enum radio_bl_level radio_inc_bl(void)
{
  bit val;
  val = radio_get_bl();
  return radio_set_bl(!val);
}

enum radio_bl_level radio_set_bl(enum radio_bl_level val)
{
  _radio_set_bit(data_out, 3, 5, val);
  _radio_write_data();
  _radio_read_data(); // 填充data_in
  return val;
}

void radio_write_rom_bl(void)
{
  rom_write(ROM_RADIO_BL, radio_get_bl());
}

bit radio_get_hcc(void)
{
  return _radio_get_bit(data_out, 3, 2);
}

bit radio_set_hcc(bit val)
{
  _radio_set_bit(data_out, 3, 2, val);
  _radio_write_data();
  _radio_read_data(); // 填充data_in
  return val;
}

void radio_write_rom_hcc(void)
{
  rom_write(ROM_RADIO_BL, radio_get_bl());
}

bit radio_get_snc(void)
{
  return _radio_get_bit(data_out, 3, 1);
}

bit radio_set_snc(bit val)
{
  _radio_set_bit(data_out, 3, 1, val);
  _radio_write_data();
  _radio_read_data(); // 填充data_in
  return val;
}

void radio_write_rom_snc(void)
{
  rom_write(ROM_RADIO_SNC, radio_get_snc());
}

enum radio_dtc_level radio_set_dtc(enum radio_dtc_level val)
{
  _radio_set_bit(data_out, 4, 6, val);
  _radio_write_data();
  _radio_read_data(); // 填充data_in
  return val;
}

enum radio_dtc_level radio_get_dtc(void)
{
  return _radio_get_bit(data_out, 4, 6);
}

enum radio_dtc_level radio_inc_dtc(void)
{
  bit val;
  val = _radio_get_bit(data_out, 4, 6);
  return radio_set_dtc(!val);
}

void radio_write_rom_dtc(void)
{
  rom_write(ROM_RADIO_DTC, radio_get_dtc());
}

void radio_dump(void)
{
  unsigned char i;
  CDBG(("radio: %s\n", radio_enabled ? "ON" : "OFF"));
  if(radio_enabled) {
    CDBG(("  freq: %u\n", freqency));
    CDBG(("  vol: %bu\n", volume));
    CDBG(("  hlsi: %s\n", radio_get_hlsi() ? "HI" : "LO"));
    CDBG(("  ms: %s\n", radio_get_ms() ?  "ON" : "OFF"));
    CDBG(("  bl: %s\n", radio_get_bl() == RADIO_BL_JAPNESE ? "JAPNESE" : "EUROPE"));
    CDBG(("  hcc: %s\n", radio_get_hcc() ? "ON" : "OFF"));
    CDBG(("  snc: %s\n", radio_get_snc() ? "ON" : "OFF"));
    CDBG(("  dtc: %s\n", radio_get_dtc() == RADIO_DTC_75US ? "75us" : "50us")); 

    CDBG(("raw data_out:\n"));
    for(i = 0 ; i < 5; i ++) {
      CDBG(("  %02bx", data_out[i]));
    }
    CDBG(("\nraw data_in:\n"));
    for(i = 0 ; i < 5; i ++) {
      CDBG(("  %02bx", data_in[i]));
    }
    CDBG(("\n"));
  }
 
}