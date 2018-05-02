#include "thermo.h"
#include "debug.h"
#include "task.h"
#include "serial_hub.h"
#include "sm.h"
#include "i2c.h"
#include "misc.h"
#include "power.h"

#define THERMO_THRESHOLED_MAX (85)
#define THERMO_THRESHOLED_MIN (-55)

#define THERMO_THRESHOLED_STEP 1

static bit thermo_hi_enabled;
static bit thermo_lo_enabled;

#ifdef __CLOCK_EMULATE__

#define THERMO_HI_I2C_ADDRESS  0x90 //1001 0000
#define THERMO_LO_I2C_ADDRESS  0x92 //1001 0010
#define THERMO_HUB_I2C_ADDRESS  0x48 //0100 1000

static void thermo_power_on(void)
{
  unsigned int val;
  CDBG("thermo_power_on\n");
  
  // stop thermo T
  I2C_Put(THERMO_HI_I2C_ADDRESS, 0x22, 0);
  I2C_Put(THERMO_LO_I2C_ADDRESS, 0x22, 0);  
  
  // set th and tl
  thermo_hi_threshold_reset(); 
  thermo_lo_threshold_reset();

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

  thermo_hi_enabled = thermo_lo_enabled = 0;

  delay_ms(10); // delay 10 ms

  // 设置thermo hub
  // Configuration Register 设置为全1，用于input
  I2C_Put(THERMO_HUB_I2C_ADDRESS, 0x3, 0xFF);
  
  // Polarity Inversion Register 设置为全0
  I2C_Put(THERMO_HUB_I2C_ADDRESS, 0x2, 0x0);

  // 读取一次端口寄存器消除中断
  I2C_Get(THERMO_HUB_I2C_ADDRESS, 0x0, (unsigned char *)&val);
  CDBG("thermo int reg is %bx\n", val);
}

static void thermo_power_reset(void)
{
  // Configuration Register 设置为全0，用于output
  I2C_Put(THERMO_HUB_I2C_ADDRESS, 0x6, 0x0);
  
  // Polarity Inversion Register 设置为全0
  I2C_Put(THERMO_HUB_I2C_ADDRESS, 0x7, 0x0);
  
  // Configuration Register 设置为全0，用于output
  I2C_Put(THERMO_HUB_I2C_ADDRESS, 0x4, 0x0);
  
  // Polarity Inversion Register 设置为全0
  I2C_Put(THERMO_HUB_I2C_ADDRESS, 0x5, 0x0);
  
  serial_ctl_out();
}

static void thermo_power_off(void)
{
  CDBG("thermo_power_off\n");
  thermo_power_reset();
  // stop thermo T
  I2C_Put(THERMO_HI_I2C_ADDRESS, 0x22, 0);
  I2C_Put(THERMO_LO_I2C_ADDRESS, 0x22, 0); 
}

void thermo_initialize (void)
{
  CDBG("thermo_initialize\n");
  thermo_hi_enabled = thermo_lo_enabled = 0;
  thermo_power_reset();
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

char thermo_hi_threshold_get()
{
  unsigned int val;
  
  I2C_Gets(THERMO_HI_I2C_ADDRESS, 0xA1, 2, (unsigned char *)&val);
  return thermo_hex_to_temp(val);
}

void thermo_hi_threshold_set(char temp)
{
  unsigned int val;
  CDBG("thermo_hi_threshold_set %bd\n", temp);

  if (temp > THERMO_THRESHOLED_MAX)
    temp = THERMO_THRESHOLED_MAX;
  
  if(temp < THERMO_THRESHOLED_MIN)
    temp = THERMO_THRESHOLED_MIN;
  
  val = thermo_temp_to_hex(temp);
  I2C_Puts(THERMO_HI_I2C_ADDRESS, 0xA1, 2, (unsigned char *)&val);
  val = thermo_temp_to_hex(temp);
  I2C_Puts(THERMO_HI_I2C_ADDRESS, 0xA2, 2, (unsigned char *)&val);
}

void thermo_lo_threshold_set(char temp)
{
  unsigned int val;
  CDBG("thermo_lo_threshold_set %bd\n", temp);
  
  if (temp > THERMO_THRESHOLED_MAX)
    temp = THERMO_THRESHOLED_MAX;
  
  if(temp < THERMO_THRESHOLED_MIN)
    temp = THERMO_THRESHOLED_MIN;
  
  val = thermo_temp_to_hex(temp);
  I2C_Puts(THERMO_LO_I2C_ADDRESS, 0xA1, 2, (unsigned char *)&val);
  val = thermo_temp_to_hex(temp);
  I2C_Puts(THERMO_LO_I2C_ADDRESS, 0xA2, 2, (unsigned char *)&val);
}

char thermo_lo_threshold_get()
{
  unsigned int val;
  
  I2C_Gets(THERMO_LO_I2C_ADDRESS, 0xA1, 2, (unsigned char *)&val);
  return thermo_hex_to_temp(val);
}

// 一次性测温函数
char thermo_get_current(void)
{
  unsigned int val;
  char ret;
  
  I2C_Gets(THERMO_HI_I2C_ADDRESS, 0xAA, 2, (unsigned char *)&val);
  CDBG("get current temp return %x\n", val);
  
  ret = (char)((val >> 8) & 0xFF); // 整数部分
  CDBG("current temp is %bd\n", ret);
  return ret;
}

#else

// TMP 101 mode
#define THERMO_I2C_ADDRESS 0x90 //1001 0000

static void thermo_power_off(void)
{
  unsigned char val;
  
//  I2C_Get(THERMO_I2C_ADDRESS, 0x1, &val);
//  val |= 0x1;
  val = 0x03;
  I2C_Put(THERMO_I2C_ADDRESS, 0x1, val);
}

static void thermo_power_on(void)
{
  unsigned char val;
  
  // genenal call reset
  I2C_Get(0x0, 0x6, &val);
  
  // TLOW Register
  CDBG("before thermo lo reset: %bd\n", thermo_lo_threshold_get());
  thermo_lo_threshold_reset();
  CDBG("after thermo lo reset: %bd\n", thermo_lo_threshold_get());
  
  // THIGH Register
  CDBG("before thermo hi reset: %bd\n", thermo_hi_threshold_get());
  thermo_hi_threshold_reset();
  CDBG("before thermo hi reset: %bd\n", thermo_hi_threshold_get());
  
  //OS/ALERT R1 R0 F1 F0 POL TM SD
  // R1 R0 = 0 0 : 9 bits (0.5 degree), 40ms
  // F1 F0 = 0 0 : , Fault Queue = 0
  // interrupt active low = 0
  // interrupt mode on  = 1
  // shutdown mode off = 0
  val = 0x2; // 00000010
  I2C_Put(THERMO_I2C_ADDRESS, 0x1, val);
}


void thermo_initialize (void)
{
  unsigned char val;
  CDBG("thermo_initialize\n");
  
  thermo_hi_enabled = thermo_lo_enabled = 0;
  
  thermo_power_on();
  
  thermo_power_off();
}

void scan_thermo(void)
{
  char hi, lo, current;
  bit has_event = 0;
  
  // 读取一次端口寄存器消除中断
  hi = thermo_hi_threshold_get();
  lo = thermo_lo_threshold_get();
  current = thermo_get_current();
  CDBG("scan_thermo, hi = %bd, lo = %bd, current = %bd\n", hi, lo, current);
  
  if(current >= hi && thermo_hi_enabled) {
    CDBG("EV_THERMO_HI!\n");
    set_task(EV_THERMO_HI);
    has_event = 1;
  } else if(current <= lo && thermo_lo_enabled) {
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
//  temp = val & 0xFF;
  temp = (val >> 8) & 0xFF;
  CDBG("thermo_hex_to_temp x%x -> %bd\n", val, temp);
  return temp;
}

char thermo_hi_threshold_get()
{
  unsigned int val;
  I2C_Gets(THERMO_I2C_ADDRESS, 0x3, 2, (unsigned char *)&val);
  return thermo_hex_to_temp(val);
}

char thermo_lo_threshold_get()
{
  unsigned int val;
  I2C_Gets(THERMO_I2C_ADDRESS, 0x2, 2, (unsigned char *)&val);
  return thermo_hex_to_temp(val);
}


void thermo_hi_threshold_set(char temp)
{
  unsigned int val;
  CDBG("thermo_hi_threshold_set %bd\n", temp);
  
  if (temp > THERMO_THRESHOLED_MAX)
    temp = THERMO_THRESHOLED_MAX;
  
  if(temp < THERMO_THRESHOLED_MIN)
    temp = THERMO_THRESHOLED_MIN;
  
  val = thermo_temp_to_hex(temp);
  I2C_Puts(THERMO_I2C_ADDRESS, 0x3, 2, (unsigned char *)&val);
}

void thermo_lo_threshold_set(char temp)
{
  unsigned int val;
  CDBG("thermo_lo_threshold_set %bd\n", temp);
  
  if (temp > THERMO_THRESHOLED_MAX)
    temp = THERMO_THRESHOLED_MAX;
  
  if(temp < THERMO_THRESHOLED_MIN)
    temp = THERMO_THRESHOLED_MIN;
  
  val = thermo_temp_to_hex(temp);
  I2C_Puts(THERMO_I2C_ADDRESS, 0x2, 2, (unsigned char *)&val);
}

char thermo_get_current(void)
{
  unsigned int val;
  unsigned char val1;
  
  // one shot 
  I2C_Get(THERMO_I2C_ADDRESS, 0x1, &val1);
  val1 |= 0x80;
  I2C_Put(THERMO_I2C_ADDRESS, 0x1, val1);
  
  delay_ms(50); // delay 50 ms
  
  I2C_Gets(THERMO_I2C_ADDRESS, 0x0, 2, (unsigned char *)&val);
  
  return thermo_hex_to_temp(val);
}

#endif

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
// 连续调节温度，从THERMO_THRESHOLED_MIN->THERMO_THRESHOLED_MAX->THERMO_THRESHOLED_INVALID->THERMO_THRESHOLED_MIN
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

void thermo_enable(bit enable)
{
  if(enable) {
    thermo_power_on();
  } else {
    thermo_power_off();
		thermo_hi_enabled = thermo_hi_enabled = 0;
  }
}

void thermo_proc(enum task_events ev)
{
  CDBG("thermo_proc\n");
  run_state_machine(ev);
}

void thermo_hi_enable(bit enable)
{
	CDBG("thermo_hi_enable %bd\n", enable ? 1 : 0);
  
  thermo_hi_enabled = enable;
}

void thermo_lo_enable(bit enable)
{
	CDBG("thermo_lo_enable %bd\n", enable ? 1 : 0);
  
  thermo_lo_enabled = enable;
}

void thermo_enter_powersave(void)
{
  CDBG("thermo_enter_powersave\n");
}

void thermo_leave_powersave(void)
{
  CDBG("thermo_leave_powersave\n");
}