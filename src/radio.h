#ifndef __CLOCK_RADIO_H__
#define __CLOCK_RADIO_H__

void radio_initialize (void);
void radio_enter_powersave(void);
void radio_leave_powersave(void);
void radio_enable(bit enable);

typedef void (code *RADIO_CB_PROC)(unsigned int);

unsigned int radio_prev_station(RADIO_CB_PROC cb);
unsigned int radio_next_station(RADIO_CB_PROC cb);
unsigned int radio_dec_frequency(void);
unsigned int radio_inc_frequency(void);
unsigned int radio_get_frequency(void);
void radio_write_rom_frequency(void);
unsigned char radio_dec_volume(void);
unsigned char radio_inc_volume(void);
unsigned char radio_get_volume(void);
unsigned char radio_set_volume(unsigned char val);
void radio_write_rom_volume(void);

bit radio_get_hlsi(void);
bit radio_set_hlsi(bit val);
void radio_write_rom_hlsi(void);

bit radio_get_ms(void);
bit radio_set_ms(bit val);
void radio_write_rom_ms(void);

enum radio_bl_level
{
  RADIO_BL_JAPNESE,
  RADIO_BL_EUROPE
};
enum radio_bl_level radio_set_bl(enum radio_bl_level val);
enum radio_bl_level radio_get_bl(void);
enum radio_bl_level radio_inc_bl(void);
void radio_write_rom_bl(void);

bit radio_get_hcc(void);
bit radio_set_hcc(bit val);
void radio_write_rom_hcc(void);

bit radio_get_snc(void);
bit radio_set_snc(bit val);
void radio_write_rom_snc(void);

enum radio_dtc_level
{
  RADIO_DTC_75US,
  RADIO_DTC_50US
};
enum radio_dtc_level radio_set_dtc(enum radio_dtc_level dtc);
enum radio_dtc_level radio_get_dtc(void);
enum radio_dtc_level radio_inc_dtc(void);
void radio_write_rom_dtc(void);

#endif