#include <STC89C5xRC.H>

#include "power.h"
#include "task.h"
#include "sm.h"
#include "clock.h"
#include "com.h"
#include "rtc.h"
#include "key.h"
#include "led.h"
#include "alarm.h"
#include "timer.h"
#include "beeper.h"
#include "int_hub.h"
#include "debug.h"
#include "cext.h"
#include "misc.h"
#include "rom.h"
#include "fuse.h"
#include "hg.h"
#include "remote.h"
#include "mpu.h"
#include "lt_timer.h"
#include "thermo.h"
#include "i2c.h"
#include "delay_task.h"
#include "radio.h"

sbit POWER_3_3V_EN = P3 ^ 5;
sbit POWER_5V_EN   = P3 ^ 6;

//10101000
#define POWER_I2C_ADDR 0xA8

// 最大测量范围：0~5.6V, LSB = 1.3675213675213675f mv
#define POWER_MILL_VOLTAGE_PER_LSB  1.3675213675213675f

#define POWER_VALUE_SLOT 5

#define POWER_DELAY_INIT_SEC 10

#define POWER_HIGH_ALERT_VOLTAGE 500 // 5.0V
#define POWER_LOW_ALERT_VOLTAGE  330 // 3.3V
#define POWER_LOW_POWERPACK_ALERT_VOLTAGE 300 // 3.0V

struct ocv_slot {
  unsigned int ocv_hex;
  unsigned char percent;
};


#include "ocv.c"

static unsigned int power_value[POWER_VALUE_SLOT];
static unsigned char cur_index;

static unsigned char powersave_to_s;
static unsigned char last_ps_s;
static bit powersave_flag;
static bit is_calibration;
// v[0] = D15~D8, we need D11~D8 as hi 4 bits
// v[1] = D7~D0, we need D7~D0 as low 8 bits
static unsigned int power_pack2hex(unsigned char * v)
{
  unsigned int val;
  val = (v[0] & 0xF);
  val = val << 8;
  val |= v[1];
  CDBG(("power_pack2hex 0x%02bx 0x%02bx -> 0x%04x\n", v[0], v[1], val));
  return val;
}

static void power_hex2pack(unsigned int val, unsigned char * v)
{
  v[0] = (unsigned char)((val & 0xF00) >> 8) & 0xF;
  v[1] = (unsigned char)(val & 0xFF);
  CDBG(("power_hex2pack 0x%04x -> 0x%02bx 0x%02bx\n", val, v[0], v[1]));
}

// 浮点数电压表示为hex值
unsigned int power_float2hex(unsigned int flt)
{
  unsigned int val;
  val = (unsigned int)((float)(flt * 10) / POWER_MILL_VOLTAGE_PER_LSB);
  
  CDBG(("power_float2hex %u -> 0x%04x\n", flt, val));
  return val;
}

// hex值表示为浮点数电压
unsigned int power_hex2float(unsigned int hex)
{
  float tmp;
  unsigned int val;
  tmp = hex * POWER_MILL_VOLTAGE_PER_LSB / 10.0;
  val = (unsigned int) tmp;
  
  CDBG(("power_hex2float 0x%04x -> %u\n", hex, val));
  return val;
}

static void power_delay_task(void)
{
  CDBG(("power_delay_task\n"));
  if(!is_calibration) {
    power_enable_alert(1);
  } else {
    CDBG(("WARN: battery monitor is OFF!\n"));
  }
}

void power_load_rom(void)
{
  unsigned char powersave_to;
  
  powersave_to = rom_read(ROM_POWERSAVE_TO);
  switch(powersave_to) {
    case POWERSAVE_OFF: powersave_to_s = 0;  break;
    case POWERSAVE_30S: powersave_to_s = 30; break;
    case POWERSAVE_60S: powersave_to_s = 60; break;
  }
}

void power_initialize(void)
{

  CDBG(("power_initialize\n"));

  powersave_flag = 0;
  POWER_5V_EN = 1;
  POWER_3_3V_EN = 1;
  
  cur_index = 0;
  is_calibration = 0;
  
  // 初始化ADC
  power_load_rom();
  
  // Configuration Register, clear
  I2C_Put(POWER_I2C_ADDR, 0x3, 0);
  
  // VHIGH -- Alert Limit Register - Over Range
  // 大于5.00V, 关机！
  power_set_alert_vhigh(power_float2hex(POWER_HIGH_ALERT_VOLTAGE));
  
  // VLOW -- Alert Limit Register - Under Range
  // 小于3.00V, 关机！
  power_set_alert_vlow(power_float2hex(POWER_LOW_ALERT_VOLTAGE));
  
  // VHYST -- Alert Hysteresis Register
  power_set_hyst(0);

#ifndef __CLOCK_EMULATE__   
  // initialize interrupt
  // Cycle Time = 000
  // Alert Hold = 1
  // Alert Flag Enable = 0
  // Alert Pin Enable =  0
  // Reserved = 0
  // Polarity = 0 (active low)
  I2C_Put(POWER_I2C_ADDR, 0x2, 0x10);
#endif  
  
  
  power_set_cycle(POWER_CYCLE_2048T);
  
  power_clr_high_alert();
  power_clr_low_alert();
  
  delay_task_reg(DELAY_TASK_POWER, power_delay_task, POWER_DELAY_INIT_SEC);
}

void power_enable_alert(bit enable)
{
  unsigned char val;
  CDBG(("power_enable_alert %bu\n", enable ? 1 : 0));
  // Configuration Register, enable interrupt
  // Cycle Time = 111
  // Alert Hold = 1
  // Alert Flag Enable = 0
  // Alert Pin Enable = 1 / 0
  // Reserved = 0
  // Polarity = 0 (active low)
#ifndef __CLOCK_EMULATE__
  I2C_Get(POWER_I2C_ADDR, 0x2, &val);
  val &= ~0x4;
  if(enable)
    val |= 0x4;
  I2C_Put(POWER_I2C_ADDR, 0x2, val);
#else
  UNUSED_PARAM(val);
  UNUSED_PARAM(enable);  
#endif
}

void power_set_cycle(enum power_cycle_interval t)
{
  unsigned char val;
  CDBG(("power_set_cycle %bu\n", t));
#ifndef __CLOCK_EMULATE__  
  I2C_Get(POWER_I2C_ADDR, 0x2, &val);
  val &= ~0xE0;
  val |= ((t << 5) & 0xE0);
  I2C_Put(POWER_I2C_ADDR, 0x2, val);
#else
  UNUSED_PARAM(val);
  UNUSED_PARAM(t);  
#endif
}

enum power_cycle_interval power_get_cycle(void)
{
  unsigned char val;
#ifndef __CLOCK_EMULATE__  
  I2C_Get(POWER_I2C_ADDR, 0x2, &val);
  CDBG(("power_get_cycle return %bu\n", ((val & 0xE0) >> 5) & 0x7));
  return (val & 0x7);
#else
  UNUSED_PARAM(val);
  return 0;
#endif
}


void power_set_alert_vhigh(unsigned int val)
{
  unsigned char v[2];
  CDBG(("power_set_alert_vhigh %u\n", val));
  power_hex2pack(val, v);
#ifndef __CLOCK_EMULATE__ 
  I2C_Puts(POWER_I2C_ADDR, 0x4, 2, v);
#endif
}

unsigned int power_get_alert_vhigh(void)
{
  unsigned char v[2];
#ifndef __CLOCK_EMULATE__   
  I2C_Gets(POWER_I2C_ADDR, 0x4, 2, v);
#else
  v[0] = v[1] = 0;
#endif
  CDBG(("power_get_alert_vhigh 0x%02bx 0x%02bx\n", v[0], v[1]));
  return power_pack2hex(v); 
}

void power_set_alert_vlow(unsigned int val)
{
  unsigned char v[2];
  CDBG(("power_set_alert_vlow %bu\n", val));
  power_hex2pack(val, v);
#ifndef __CLOCK_EMULATE__ 
  I2C_Puts(POWER_I2C_ADDR, 0x3, 2, v);
#endif
}

unsigned int power_get_alert_vlow(void)
{
  unsigned char v[2];
#ifndef __CLOCK_EMULATE__ 
  I2C_Gets(POWER_I2C_ADDR, 0x3, 2, v);
#else
  v[0] = v[1] = 0;
#endif
  CDBG(("power_get_alert_vlow 0x%02bx 0x%02bx\n", v[0], v[1]));
  return power_pack2hex(v); 
}

void power_set_hyst(unsigned int val)
{
  unsigned char v[2];
  CDBG(("power_set_hyst %bu\n", val));
  power_hex2pack(val, v); 
#ifndef __CLOCK_EMULATE__   
  I2C_Puts(POWER_I2C_ADDR, 0x5, 2, v);
#endif
}

unsigned int power_get_hyst(void)
{
  unsigned char v[2];
#ifndef __CLOCK_EMULATE__   
  I2C_Gets(POWER_I2C_ADDR, 0x5, 2, v);
#else
  v[0] = v[1] = 0;
#endif
  CDBG(("power_get_hyst 0x%02bx 0x%02bx\n", v[0], v[1]));
  return power_pack2hex(v); 
}

void power_clr_high_alert(void)
{
  unsigned char val;
  CDBG(("power_clr_high_alert\n"));
#ifndef __CLOCK_EMULATE__   
  I2C_Get(POWER_I2C_ADDR, 0x1, &val);
  val |= 0x2; // The controller writes a one to this bit. ???
  I2C_Put(POWER_I2C_ADDR, 0x1, val);
#else
  UNUSED_PARAM(val);
#endif
}

void power_clr_low_alert(void)
{
  unsigned char val;
  CDBG(("power_clr_low_alert\n"));
#ifndef __CLOCK_EMULATE__   
  I2C_Get(POWER_I2C_ADDR, 0x1, &val);
  val |= 0x1; // The controller writes a one to this bit. ???
  I2C_Put(POWER_I2C_ADDR, 0x1, val);
#else
  UNUSED_PARAM(val);
#endif
}

bit power_test_high_alert(void)
{
  unsigned char val;
#ifndef __CLOCK_EMULATE__     
  I2C_Get(POWER_I2C_ADDR, 0x1, &val);
#else
  val = 0;
#endif
  CDBG(("power_test_high_alert return %s\n", (val & 0x2) != 0 ? "1" : "0"));
  return (val & 0x2) != 0;
}

bit power_test_low_alert(void)
{
  unsigned char val;
#ifndef __CLOCK_EMULATE__   
  I2C_Get(POWER_I2C_ADDR, 0x1, &val);
#else
  val = 0;
#endif
  CDBG(("power_test_low_alert return %s\n", (val & 0x1) != 0 ? "1" : "0"));
  return (val & 0x1) != 0;
}

void power_set_calibration(bit enable)
{
  CDBG(("power_set_calibration %bu\n", enable ? 1 : 0));
  is_calibration = enable;
}

bit power_get_calibration(void)
{
  return is_calibration;
}

void scan_power(void)
{
  CDBG(("scan_power\n"));
  
  if(power_test_high_alert()) {
    power_clr_high_alert();
    CDBG(("HIGH POWER -> POWER OFF!\n"));
    power_3_3v_enable(0);
  }
  
  if(power_test_low_alert()) {
    power_clr_low_alert();
    CDBG(("LOW POWER -> POWER OFF!\n"));
    power_3_3v_enable(0);
  }
}

void power_proc(enum task_events ev)
{
  run_state_machine(ev);
}

void power_enter_powersave(void)
{
  CDBG(("power_enter_powersave\n"));
  power_set_flag();
  led_enter_powersave(); 
  timer_enter_powersave(); 
  lt_timer_enter_powersave();
  clock_enter_powersave();
  rtc_enter_powersave();       
  key_enter_powersave();                
  alarm_enter_powersave();       
  beeper_enter_powersave();
  fuse_enter_powersave();
  hg_enter_powersave();
  mpu_enter_powersave();
  thermo_enter_powersave();
  remote_enter_powersave();
  radio_enter_powersave();
  com_enter_powersave();
  while(power_test_flag()) {
#ifdef __CLOCK_EMULATE__    
    PCON |= 0x1; // proteus 不支持0x2
#else
    PCON |= 0x2;
#endif
    com_leave_powersave(); 
    scan_int_hub_proc(EV_SCAN_INT_HUB);
    com_enter_powersave();
    if(!power_test_flag()) {
      break;
    }
  }
}

void power_leave_powersave(void)
{

  com_leave_powersave(); 
  radio_leave_powersave();
  remote_leave_powersave();
  thermo_leave_powersave();
  mpu_leave_powersave();
  hg_leave_powersave();
  fuse_leave_powersave();
  beeper_leave_powersave();
  alarm_leave_powersave();   
  key_leave_powersave(); 
  rtc_leave_powersave(); 
  clock_leave_powersave();
  lt_timer_leave_powersave();
  timer_leave_powersave();
  led_leave_powersave();
  CDBG(("power_leave_powersave\n"));
}

unsigned char power_get_powersave_to_s(void)
{
  return powersave_to_s;
}

enum powersave_time power_get_powersave_to(void)
{
  switch(powersave_to_s) {
    case 0:  return POWERSAVE_OFF;
    case 30: return POWERSAVE_30S;
    case 60: return POWERSAVE_60S;
  }
  return POWERSAVE_OFF;
}

void power_inc_powersave_to(void)
{
  switch(powersave_to_s) {
    case 0:
      powersave_to_s = 15;  break;
    case 15: 
      powersave_to_s = 30; break;
    case 30: 
      powersave_to_s = 0; break;
  }
}

void power_write_rom_powersave_to(void)
{
  rom_write(ROM_POWERSAVE_TO, power_get_powersave_to());
}

bit power_test_powersave_to(void)
{
  if(powersave_to_s != 0 
    && time_diff_now(last_ps_s) >= powersave_to_s) {
      CDBG(("test_powersave_to time out!\n"));
      set_task(EV_POWER_SAVE);
      return 1;
  }
  return 0;
}

bit power_test_flag(void)
{
  return powersave_flag;
}

void power_set_flag(void)
{
  powersave_flag = 1;
}

void power_clr_flag(void)
{
  powersave_flag = 0;
}

void power_reset_powersave_to(void)
{
  last_ps_s = clock_get_sec_256();
}

void power_3_3v_enable(bit enable)
{
  CDBG(("power_3_3v_enable %bu\n", enable ? 1 : 0));
  
  POWER_3_3V_EN = enable;
}

bit power_3_3v_get_enable(void)
{
  return POWER_3_3V_EN;
}

void power_5v_enable(bit enable)
{
  CDBG(("power_5v_enable %bu\n", enable ? 1 : 0));
  
  if(enable) {
    power_set_alert_vlow(power_float2hex(POWER_LOW_POWERPACK_ALERT_VOLTAGE));
    POWER_5V_EN = !enable;
  } else {
    POWER_5V_EN = !enable;
    power_set_alert_vlow(power_float2hex(POWER_LOW_ALERT_VOLTAGE));
  }
}

bit power_5v_get_enable(void)
{
  return !POWER_5V_EN;
}

unsigned char power_hex2percent(unsigned int hex)
{
  unsigned char i = 0;
  unsigned long val = 0;
  bit found = 0;
  
  if(hex >= ocv_table[0].ocv_hex)
    val =  100;
  else if(hex < ocv_table[sizeof(ocv_table)/sizeof(struct ocv_slot) - 1].ocv_hex)
    val =  0;
  else {
    for(i = 0 ; i < (sizeof(ocv_table)/sizeof(struct ocv_slot) - 1); i ++) {
      if(hex < ocv_table[i].ocv_hex && hex >= ocv_table[i + 1].ocv_hex) {
        found = 1;
        break;
      }
    }
    if(found) {
      val = (hex - ocv_table[i + 1].ocv_hex) * 100;
      val = val / (ocv_table[i].ocv_hex - ocv_table[i + 1].ocv_hex);
      val = val * (ocv_table[i].percent - ocv_table[i + 1].percent) / 100;
      val += ocv_table[i + 1].percent;
    }
  }
  CDBG(("power_hex2percent: hex %u-> percent %bu\n", hex, (unsigned char)val));
  return (unsigned char)val;
}
/*
unsigned char power_hex2percent(unsigned int hex)
{
  unsigned int full;
  unsigned int empty;
  unsigned long val;
  
  full = power_float2hex(voltage_full);
  empty = power_float2hex(voltage_empty);
 
  if (hex <= empty) {
    val = 0;
  } else if (hex >= full) {
    val = 100;
  } else {
    val = (hex - empty) * 100;
    val = val / (full - empty);
  }
  CDBG("power_hex2percent: empty = %u, full = %u, hex = %u, val = %Lu\n",
    empty, full, hex, val);
  return (unsigned char)(val);
}
*/
unsigned int power_get_hex(void)
{
  unsigned char v[2];
  unsigned int val;
  unsigned long hex;
#ifdef __CLOCK_EMULATE__
  UNUSED_PARAM(val);
  UNUSED_PARAM(hex);
  UNUSED_PARAM(v[0]);
  return 3060;
#else
  I2C_Gets(POWER_I2C_ADDR, 0x0, 2, v);
  CDBG(("adc return %02bx %02bx\n", v[0], v[1]));
  
  val = power_pack2hex(v);
  CDBG(("power_pack2hex return 0x%04x\n", val));
  
  power_value[cur_index] = val;
  cur_index = (++cur_index) % POWER_VALUE_SLOT;
  
  hex = 0;
  for(val = 0 ; val < POWER_VALUE_SLOT; val ++)
  {
    hex += power_value[val];
  }
  hex = hex / POWER_VALUE_SLOT;
  CDBG(("power_get_hex return 0x%04x\n", (unsigned int)hex));
  return (unsigned int)hex;
#endif
}

unsigned int power_get_voltage(void)
{
  unsigned int hex;
  hex = power_get_hex();
  return power_hex2float(hex);
}

// return 0~100
unsigned char power_get_percent(void)
{ 
  unsigned int hex;
  unsigned int flt;
  hex = power_get_hex();
  flt = power_hex2float(hex);
  CDBG(("power_hex2float: %u (10mV)\n", flt));
  return power_hex2percent(hex);
}
