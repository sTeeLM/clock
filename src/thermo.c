#include "thermo.h"
#include "debug.h"
#include "task.h"
#include "serial_hub.h"
#include "sm.h"
#include "i2c.h"
#include "misc.h"
#include "power.h"

#define THERMO_HI_I2C_ADDRESS  0x90 //1001 0000
#define THERMO_LO_I2C_ADDRESS  0x92 //1001 0010
#define THERMO_HUB_I2C_ADDRESS  0x48 //0100 1000

#define THERMO_THRESHOLED_MAX (85)
#define THERMO_THRESHOLED_MIN (-55)

#define THERMO_THRESHOLED_STEP 5

static bit thermo_hi_enabled;
static bit thermo_lo_enabled;

static void thermo_power_on(void)
{
  unsigned int val;
  CDBG("thermo_power_on\n");
  
  serial_set_ctl_bit(SERIAL_BIT_THERMO_EN, 0);
  
  serial_ctl_out();
  
  // stop thermo T
  I2C_Put(THERMO_HI_I2C_ADDRESS, 0x22, 0);
  I2C_Put(THERMO_LO_I2C_ADDRESS, 0x22, 0);  
  
  // set th and tl
  thermo_hi_threshold_reset(); 
  thermo_lo_threshold_reset();
  
  // Configuration Register 设置为全1，用于input
  I2C_Put(THERMO_HUB_I2C_ADDRESS, 0x3, 0xFF);
  
  // Polarity Inversion Register 设置为全0
  I2C_Put(THERMO_HUB_I2C_ADDRESS, 0x2, 0x0);

  // DONE|THF|TLF|NVB|1|0|POL|1SHOT
  // 0|0|0|0|0|0|0|0
  // set thermo0 config
  I2C_Put(THERMO_HI_I2C_ADDRESS, 0xAC, 0x00);
  // 0|0|0|0|0|0|1|0
  // set thermo1 config
  I2C_Put(THERMO_LO_I2C_ADDRESS, 0xAC, 0x02);

  // 开启测温
  I2C_Put(THERMO_HI_I2C_ADDRESS, 0xEE, 0);
  I2C_Put(THERMO_LO_I2C_ADDRESS, 0xEE, 0);

  delay_ms(10); // delay 10 ms

  // 读取一次端口寄存器消除中断
  I2C_Get(THERMO_HUB_I2C_ADDRESS, 0x0, (unsigned char *)&val);
  CDBG("thermo int reg is %bx\n", val);
}

static void thermo_power_off(void)
{
  CDBG("thermo_power_off\n");
  serial_set_ctl_bit(SERIAL_BIT_THERMO_EN, 1);
  serial_ctl_out();
}

void thermo_initialize (void)
{
  CDBG("thermo_initialize\n");
  thermo_hi_enabled = thermo_lo_enabled = 0;
}

void thermo_enter_powersave(void)
{
  CDBG("thermo_enter_powersave\n");
}

void thermo_leave_powersave(void)
{
  CDBG("thermo_leave_powersave\n");
}

void scan_thermo(void)
{
  unsigned char val;
  
  bit has_event = 0;
  
  CDBG("scan_thermo\n");
  
  // 读取一次端口寄存器消除中断
  I2C_Get(THERMO_HUB_I2C_ADDRESS, 0x0, &val);
  CDBG("thermo int reg is %bx\n", val);
  
  if((val & 0x1) == 0 && thermo_hi_enabled) {
    CDBG("EV_THERMO_HI!\n");
    set_task(EV_THERMO_HI);
    has_event = 1;
  } else if((val & 0x2) == 0 && thermo_lo_enabled) {
    CDBG("EV_THERMO_LO!\n");
    set_task(EV_THERMO_LO);
    has_event = 1;
  }
  
  if(has_event && power_test_flag()) {
    power_clr_flag();
  }
}

static unsigned int thermo_temp_to_hex(char temp)
{
  unsigned int ret = temp;
  ret &= 0xFF;
  ret <<= 8;
  CDBG("thermo_temp_to_hex %bd -> x%x\n", temp, ret);
  return ret;
}

static char thermo_hex_to_temp(unsigned int val)
{
  char temp;
  temp = (val >> 8) & 0xFF;
  CDBG("thermo_hex_to_temp x%x -> %bd\n", val, temp);
  return temp;
}

void thermo_proc(enum task_events ev)
{
  CDBG("thermo_proc\n");
  run_state_machine(ev);
}

void thermo_hi_enable(bit enable)
{
	CDBG("thermo_hi_enable %bd\n", enable ? 1 : 0);
  
  if(enable && (!thermo_hi_enabled && !thermo_lo_enabled)) {
    thermo_power_on();
  } else if(!enable && thermo_hi_enabled && !thermo_lo_enabled){
    thermo_power_off();
  }
  
  thermo_hi_enabled = enable;
}

void thermo_lo_enable(bit enable)
{
	CDBG("thermo_lo_enable %bd\n", enable ? 1 : 0);
  
  if(enable && (!thermo_hi_enabled && !thermo_lo_enabled)) {
    thermo_power_on();
  } else if(!enable && !thermo_hi_enabled && thermo_lo_enabled){
    thermo_power_off();
  }
  
  thermo_lo_enabled = enable;
}

void thermo_hi_threshold_reset()
{  
  thermo_hi_threshold_set(THERMO_THRESHOLED_MAX);
}

void thermo_lo_threshold_reset()
{
  
  thermo_lo_threshold_set(THERMO_THRESHOLED_MIN);
}

bit thermo_hi_threshold_reach_bottom()
{
  return thermo_hi_threshold_get() <= THERMO_THRESHOLED_MIN;
}

bit thermo_hi_threshold_reach_top()
{
  return thermo_hi_threshold_get() >= THERMO_THRESHOLED_MAX;
}

bit thermo_lo_threshold_reach_bottom()
{
  return thermo_lo_threshold_get() <= THERMO_THRESHOLED_MIN;
}

bit thermo_lo_threshold_reach_top()
{
  return thermo_lo_threshold_get() >= THERMO_THRESHOLED_MAX;
}

char thermo_hi_threshold_get()
{
  unsigned int val;
  
  if(!thermo_hi_enabled) return THERMO_THRESHOLED_INVALID;
  
  I2C_Gets(THERMO_HI_I2C_ADDRESS, 0xA1, 2, (unsigned char *)&val);
  return thermo_hex_to_temp(val);
}

void thermo_hi_threshold_set(char temp)
{
  unsigned int val;
  CDBG("thermo_hi_threshold_set %bd\n", temp);
  
  if(!thermo_hi_enabled) return;
  
  val = thermo_temp_to_hex(temp);
  I2C_Puts(THERMO_HI_I2C_ADDRESS, 0xA1, 2, (unsigned char *)&val);
  val = thermo_temp_to_hex(temp);
  I2C_Puts(THERMO_HI_I2C_ADDRESS, 0xA2, 2, (unsigned char *)&val);
}

void thermo_lo_threshold_set(char temp)
{
  unsigned int val;
  CDBG("thermo_lo_threshold_set %bd\n", temp);
  
  if(!thermo_lo_enabled) return;
  
  val = thermo_temp_to_hex(temp);
  I2C_Puts(THERMO_LO_I2C_ADDRESS, 0xA1, 2, (unsigned char *)&val);
  val = thermo_temp_to_hex(temp);
  I2C_Puts(THERMO_LO_I2C_ADDRESS, 0xA2, 2, (unsigned char *)&val);
}

char thermo_lo_threshold_get()
{
  unsigned int val;
  
  if(!thermo_lo_enabled) return THERMO_THRESHOLED_INVALID;
  
  I2C_Gets(THERMO_LO_I2C_ADDRESS, 0xA1, 2, (unsigned char *)&val);
  return thermo_hex_to_temp(val);
}

void thermo_hi_threshold_dec()
{
  char val;
  val = thermo_hi_threshold_get();
  if(val > THERMO_THRESHOLED_MIN) {
    val -= THERMO_THRESHOLED_STEP;
    thermo_hi_threshold_set(val);
  }
}

void thermo_hi_threshold_inc()
{
  char val;
  val = thermo_hi_threshold_get();
  if(val < THERMO_THRESHOLED_MAX) {
    val += THERMO_THRESHOLED_STEP;
    thermo_hi_threshold_set(val);
  }
}

void thermo_lo_threshold_dec()
{
  char val;
  val = thermo_lo_threshold_get();
  if(val > THERMO_THRESHOLED_MIN) {
    val -= THERMO_THRESHOLED_STEP;
    thermo_lo_threshold_set(val);
  }
}

void thermo_lo_threshold_inc()
{
  char val;
  val = thermo_lo_threshold_get();
  if(val < THERMO_THRESHOLED_MAX) {
    val += THERMO_THRESHOLED_STEP;
    thermo_lo_threshold_set(val);
  }
}

unsigned char thermo_threshold_inc(unsigned char thres)
{
  char value;
  if(thres != THERMO_THRESHOLED_INVALID) {
    value = (char)thres;
    if(value < THERMO_THRESHOLED_MAX) {
      value += THERMO_THRESHOLED_STEP;
      return (unsigned char) value;
    }else {
      return THERMO_THRESHOLED_INVALID;
    }
  } else {
    return (unsigned char)THERMO_THRESHOLED_MIN;
  }
}

// 必须在power_on后使用
char thermo_get_current(void)
{
  unsigned int val;
  char ret;
  
  if(!thermo_hi_enabled) return THERMO_THRESHOLED_INVALID;
  
  I2C_Gets(THERMO_HI_I2C_ADDRESS, 0xAA, 2, (unsigned char *)&val);
  CDBG("get current temp return %x\n", val);
  
  ret = (char)((val >> 8) & 0xFF); // 整数部分
  CDBG("current temp is %bd\n", ret);
  return ret;
}