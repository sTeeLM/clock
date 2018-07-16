#ifndef __CLOCK_POWER_H__
#define __CLOCK_POWER_H__

#include "task.h"

enum powersave_time{
  POWERSAVE_OFF  = 0,
  POWERSAVE_15S  = 1,
  POWERSAVE_30S  = 2,
  POWERSAVE_CNT
};

enum power_cycle_interval
{
  POWER_CYCLE_DISABLE = 0,
  POWER_CYCLE_32T,
  POWER_CYCLE_64T,
  POWER_CYCLE_128T,
  POWER_CYCLE_256T,
  POWER_CYCLE_512T,
  POWER_CYCLE_1024T, 
  POWER_CYCLE_2048T,   
};

void power_initialize(void);
void power_enter_powersave(void);
void power_leave_powersave(void);

unsigned char power_get_powersave_to_s(void);
enum powersave_time power_get_powersave_to(void);
void power_inc_powersave_to();
bit power_test_powersave_to(void);
void power_reset_powersave_to(void);

void power_proc(enum task_events ev);
void scan_power(void);

void power_set_flag(void);
void power_clr_flag(void);
bit power_test_flag(void);

void power_5v_enable(bit enable);
bit power_5v_get_enable(void);
void power_3_3v_enable(bit enable);
bit power_3_3v_get_enable(void);
unsigned char power_get_percent(void);

void power_set_alert_vhigh(unsigned int val);
unsigned int power_get_alert_vhigh(void);
void power_set_alert_vlow(unsigned int val);
unsigned int power_get_alert_vlow(void);
void power_set_hyst(unsigned int val);
unsigned int power_get_hyst(void);
void power_clr_high_alert(void);
void power_clr_low_alert(void);
bit power_test_high_alert(void);
bit power_test_low_alert(void);
void power_set_cycle(enum power_cycle_interval t);
enum power_cycle_interval power_get_cycle(void);
unsigned int power_float2hex(unsigned char intv, unsigned char exp);
unsigned char power_hex2float(unsigned int hex, unsigned char * intv);
unsigned char power_hex2percent(unsigned int hex);
void power_set_calibration(bit enable);
bit power_get_calibration(void);
void power_enable_alert(bit enable);
void power_load_rom(void);
unsigned char power_get_voltage(unsigned char * intv);
#endif
