#include "thermo.h"
#include "debug.h"
#include "task.h"
#include "serial_hub.h"
#include "sm.h"
#include "i2c.h"
#include "misc.h"
#include "power.h"
#include "rom.h"
#include "cext.h"

// 如果rom设置为THERMO_THRESHOLD_INVALID
// 器件设置为THERMO_THRESHOLD_MAX_INVALID
// thermo_threshold_hi 设置为THERMO_THRESHOLD_INVALID
#define THERMO_THRESHOLD_MAX_INVALID (THERMO_THRESHOLD_MAX + 1)

// 如果rom设置为THERMO_THRESHOLD_INVALID
// 器件设置为THERMO_THRESHOLD_MIN_INVALID
// thermo_threshold_lo 设置为THERMO_THRESHOLD_INVALID
#define THERMO_THRESHOLD_MIN_INVALID (THERMO_THRESHOLD_MIN - 1)

static char thermo_threshold_hi;
static char thermo_threshold_lo;

#ifdef __CLOCK_EMULATE__
// emulate mode
#define THERMO_HI_I2C_ADDRESS  0x90 //1001 0000
#define THERMO_LO_I2C_ADDRESS  0x92 //1001 0010
#define THERMO_HUB_I2C_ADDRESS  0x48 //0100 1000
#else
// TMP 101 mode
#define THERMO_I2C_ADDRESS 0x90 //1001 0000
#endif

static void thermo_load_config(void)
{
	CDBG("thermo_load_config\n");
	// 从rom中读取配置
	thermo_threshold_hi = (char)rom_read(ROM_FUSE_THERMO_HI);
	thermo_threshold_lo = (char)rom_read(ROM_FUSE_THERMO_LO);
	CDBG("thermo_threshold_hi = %bd\n", thermo_threshold_hi);
	CDBG("thermo_threshold_lo = %bd\n", thermo_threshold_lo);	
}

static void thermo_power_off(void)
{
	unsigned char val;
	
	thermo_load_config();
	
#ifdef __CLOCK_EMULATE__

  // DONE|THF|TLF|NVB|1|0|POL|1SHOT
  // 0|0|0|0|0|0|0|0
  // set thermo0 config
  I2C_Put(THERMO_HI_I2C_ADDRESS, 0xAC, 0x00);
  // 0|0|0|0|0|0|1|0
  // set thermo1 config
  I2C_Put(THERMO_LO_I2C_ADDRESS, 0xAC, 0x02);
	
	// set alert threshold
	thermo_hi_threshold_reset();
	thermo_lo_threshold_reset();	

  // stop thermo T
  I2C_Put(THERMO_HI_I2C_ADDRESS, 0x22, 0);
  I2C_Put(THERMO_LO_I2C_ADDRESS, 0x22, 0);
	
  // 设置thermo hub 为output, disable thermo
  // Configuration Register 设置为全0，用于output
  I2C_Put(THERMO_HUB_I2C_ADDRESS, 0x3, 0x0);
  
  // Polarity Inversion Register 设置为全0
  I2C_Put(THERMO_HUB_I2C_ADDRESS, 0x2, 0x0);

  // 读取一次端口寄存器消除中断
  I2C_Get(THERMO_HUB_I2C_ADDRESS, 0x0, &val);
	
#else
  // genenal call reset
  I2C_Get(0x0, 0x6, &val);
  
  // TLOW Register
  thermo_lo_threshold_reset();
  
  // THIGH Register
  thermo_hi_threshold_reset();
	
	// Shutdown Mode 
  val = 0x01;
  I2C_Put(THERMO_I2C_ADDRESS, 0x1, val);
#endif	
}

// 读取rom配置，thermo处于节电状态，中断禁止
void thermo_initialize (void)
{	
  CDBG("thermo_initialize\n");
	thermo_power_off();
}

static void thermo_power_on(void)
{
	unsigned char val;
	
	thermo_load_config();
	
#ifdef __CLOCK_EMULATE__
  // DONE|THF|TLF|NVB|1|0|POL|1SHOT
  // 0|0|0|0|0|0|0|0
  // set thermo0 config
  I2C_Put(THERMO_HI_I2C_ADDRESS, 0xAC, 0x00);
  // 0|0|0|0|0|0|1|0
  // set thermo1 config
  I2C_Put(THERMO_LO_I2C_ADDRESS, 0xAC, 0x02);
	
	// set alert threshold
	thermo_hi_threshold_reset();
	thermo_lo_threshold_reset();
	
  // 开启测温
  I2C_Put(THERMO_HI_I2C_ADDRESS, 0xEE, 0);
  I2C_Put(THERMO_LO_I2C_ADDRESS, 0xEE, 0);
	
  delay_ms(10); // delay 10 ms
	
 // 设置thermo hub
  // Configuration Register 设置为全1，用于input
  I2C_Put(THERMO_HUB_I2C_ADDRESS, 0x3, 0xFF);
  
  // Polarity Inversion Register 设置为全0
  I2C_Put(THERMO_HUB_I2C_ADDRESS, 0x2, 0x0);

  // 读取一次端口寄存器消除中断
  I2C_Get(THERMO_HUB_I2C_ADDRESS, 0x0, &val);
	
  CDBG("thermo int reg is %0xbx\n", val);
#else
  // genenal call reset
  I2C_Get(0x0, 0x6, &val);
  
  // TLOW Register
  thermo_lo_threshold_reset();
  
  // THIGH Register
  thermo_hi_threshold_reset();
  
  //OS/ALERT R1 R0 F1 F0 POL TM SD
  // R1 R0 = 0 0 : 9 bits精确度 (0.5 degree), 40ms响应时间
  // F1 F0 = 0 0 : , Fault Queue = 0
  // int active low = 0
  // Thermostat Mode/ Interrupt Mode  = 1(Interrupt Mode)
  // shutdown mode off = 0
  val = 0x2; // 00000010
  I2C_Put(THERMO_I2C_ADDRESS, 0x1, val);
#endif
}

void scan_thermo(void)
{
  unsigned char val;
	char hi,lo,current;
  
  bit has_event = 0;
  
#ifdef __CLOCK_EMULATE__
	UNUSED_PARAM(hi);
	UNUSED_PARAM(lo);	
	UNUSED_PARAM(current);	
  CDBG("scan_thermo\n");
  
  // 读取一次端口寄存器消除中断
  I2C_Get(THERMO_HUB_I2C_ADDRESS, 0x0, &val);
  CDBG("thermo int reg is %0xbx\n", val);
  
  if((val & 0x1) == 0 && (thermo_threshold_hi != THERMO_THRESHOLD_INVALID)) {
    CDBG("EV_THERMO_HI!\n");
    set_task(EV_THERMO_HI);
    has_event = 1;
  } else if((val & 0x2) == 0 && (thermo_threshold_lo != THERMO_THRESHOLD_INVALID)) {
    CDBG("EV_THERMO_LO!\n");
    set_task(EV_THERMO_LO);
    has_event = 1;
  }
#else
  // 读取一次端口寄存器消除中断
	UNUSED_PARAM(val);
  hi = thermo_hi_threshold_get();
  lo = thermo_lo_threshold_get();
  current = thermo_get_current();
  CDBG("scan_thermo, hi = %bd, lo = %bd, current = %bd\n", hi, lo, current);
  
  if(current >= hi && thermo_threshold_hi != THERMO_THRESHOLD_INVALID) {
    CDBG("EV_THERMO_HI!\n");
    set_task(EV_THERMO_HI);
    has_event = 1;
  } else if(current <= lo && thermo_threshold_lo != THERMO_THRESHOLD_INVALID) {
    CDBG("EV_THERMO_LO!\n");
    set_task(EV_THERMO_LO);
    has_event = 1;
  }
#endif
	
  if(has_event && power_test_flag()) {
    power_clr_flag();
  }
}

static unsigned int thermo_temp_to_hex(char temp)
{
  unsigned int ret = temp;
  ret &= 0xFF;
  ret <<= 8;
  CDBG("thermo_temp_to_hex %d -> 0x%x\n", temp, ret);
  return ret;
}

static char thermo_hex_to_temp(unsigned int val)
{
  char temp;
  temp = (val >> 8) & 0xFF;
  CDBG("thermo_hex_to_temp 0x%x -> %d\n", val, temp);
  return temp;
}

// 一次性测温函数
char thermo_get_current(void)
{
  unsigned int val;
	unsigned char tmp;
#ifdef __CLOCK_EMULATE__
	UNUSED_PARAM(tmp);
  I2C_Gets(THERMO_HI_I2C_ADDRESS, 0xAA, 2, (unsigned char *)&val);
  CDBG("get current temp return 0x%x\n", val);
  
  return thermo_hex_to_temp(val);;
#else
  // one shot 
  I2C_Get(THERMO_I2C_ADDRESS, 0x1, &tmp);
  tmp |= 0x80;
  I2C_Put(THERMO_I2C_ADDRESS, 0x1, tmp);
  
  delay_ms(50); // delay 50 ms
  
  I2C_Gets(THERMO_I2C_ADDRESS, 0x0, 2, (unsigned char *)&val);
	CDBG("get current temp return 0x%x\n", val);
  
  return thermo_hex_to_temp(val);
#endif
}

char thermo_hi_threshold_get()
{
  unsigned int val;
	char ret;
#ifdef  __CLOCK_EMULATE__
	I2C_Gets(THERMO_HI_I2C_ADDRESS, 0xA1, 2, (unsigned char *)&val);
#else
	I2C_Gets(THERMO_I2C_ADDRESS, 0x3, 2, (unsigned char *)&val);
#endif
  ret = thermo_hex_to_temp(val);
	if(ret == THERMO_THRESHOLD_MAX_INVALID)
		return THERMO_THRESHOLD_INVALID;
	else
		return ret;
}

char thermo_lo_threshold_get()
{
  unsigned int val;
	char ret;
#ifdef  __CLOCK_EMULATE__
	I2C_Gets(THERMO_LO_I2C_ADDRESS, 0xA1, 2, (unsigned char *)&val);
#else
  I2C_Gets(THERMO_I2C_ADDRESS, 0x2, 2, (unsigned char *)&val);
#endif
  ret = thermo_hex_to_temp(val);
	if(ret == THERMO_THRESHOLD_MIN_INVALID)
		return THERMO_THRESHOLD_INVALID;
	else
		return ret;
}


void thermo_hi_threshold_set(char temp)
{
  unsigned int val;
  CDBG("thermo_hi_threshold_set %d\n", temp);
  
	if(temp == (char)THERMO_THRESHOLD_INVALID)
		temp = THERMO_THRESHOLD_MAX_INVALID;
	else {
		if (temp > THERMO_THRESHOLD_MAX)
			temp = THERMO_THRESHOLD_MAX;
		
		if(temp < THERMO_THRESHOLD_MIN)
			temp = THERMO_THRESHOLD_MIN;
	}
  val = thermo_temp_to_hex(temp);
#ifdef  __CLOCK_EMULATE__
  I2C_Puts(THERMO_HI_I2C_ADDRESS, 0xA1, 2, (unsigned char *)&val);
  I2C_Puts(THERMO_HI_I2C_ADDRESS, 0xA2, 2, (unsigned char *)&val);
#else
	I2C_Puts(THERMO_I2C_ADDRESS, 0x3, 2, (unsigned char *)&val);
#endif
}

void thermo_lo_threshold_set(char temp)
{
  unsigned int val;
  CDBG("thermo_lo_threshold_set %d\n", temp);
  
	if(temp == (char)THERMO_THRESHOLD_INVALID)
		temp = THERMO_THRESHOLD_MIN_INVALID;
	else {
		if (temp > THERMO_THRESHOLD_MAX)
			temp = THERMO_THRESHOLD_MAX;
		
		if(temp < THERMO_THRESHOLD_MIN)
			temp = THERMO_THRESHOLD_MIN;
	}
  
  val = thermo_temp_to_hex(temp);
#ifdef  __CLOCK_EMULATE__
  I2C_Puts(THERMO_LO_I2C_ADDRESS, 0xA1, 2, (unsigned char *)&val);
  I2C_Puts(THERMO_LO_I2C_ADDRESS, 0xA2, 2, (unsigned char *)&val);
#else
  I2C_Puts(THERMO_I2C_ADDRESS, 0x2, 2, (unsigned char *)&val);
#endif
}


void thermo_hi_threshold_dec()
{
  char val;
  val = thermo_hi_threshold_get();
	
	if(val == THERMO_THRESHOLD_INVALID)
		return;
	
  if(val > THERMO_THRESHOLD_MIN) {
    val --;
    thermo_hi_threshold_set(val);
  }
}

void thermo_hi_threshold_inc()
{
  char val;
  val = thermo_hi_threshold_get();

	if(val == THERMO_THRESHOLD_INVALID)
		return;	

  if(val < THERMO_THRESHOLD_MAX) {
    val ++;
    thermo_hi_threshold_set(val);
  }
}

void thermo_lo_threshold_dec()
{
  char val;
  val = thermo_lo_threshold_get();
	
	if(val == THERMO_THRESHOLD_INVALID)
		return;	
	
  if(val > THERMO_THRESHOLD_MIN) {
    val --;
    thermo_lo_threshold_set(val);
  }
}

void thermo_lo_threshold_inc()
{
  char val;
  val = thermo_lo_threshold_get();
	
	if(val == THERMO_THRESHOLD_INVALID)
		return;	
	
  if(val < THERMO_THRESHOLD_MAX) {
    val ++;
    thermo_lo_threshold_set(val);
  }
}
// 连续调节温度，从THERMO_THRESHOLD_MIN->THERMO_THRESHOLD_MAX->THERMO_THRESHOLD_INVALID->THERMO_THRESHOLD_MIN
unsigned char thermo_threshold_inc(unsigned char thres)
{
  char value;
	unsigned char ret;
  if(thres != THERMO_THRESHOLD_INVALID) {
    value = (char)thres;
    if(value < THERMO_THRESHOLD_MAX) {
      value ++;
      ret =  (unsigned char) value;
    }else {
      ret = (unsigned char) THERMO_THRESHOLD_INVALID;
    }
  } else {
    ret = (unsigned char) THERMO_THRESHOLD_MIN;
  }
	CDBG("thermo_threshold_inc from %bd to %bd\n", (char)thres, (char)ret);
	return ret;
}

bit thermo_hi_threshold_reach_bottom()
{
  return thermo_hi_threshold_get() <= THERMO_THRESHOLD_MIN;
}

bit thermo_hi_threshold_reach_top()
{
  return thermo_hi_threshold_get() >= THERMO_THRESHOLD_MAX;
}

bit thermo_lo_threshold_reach_bottom()
{
  return thermo_lo_threshold_get() <= THERMO_THRESHOLD_MIN;
}

bit thermo_lo_threshold_reach_top()
{
  return thermo_lo_threshold_get() >= THERMO_THRESHOLD_MAX;
}

void thermo_enable(bit enable)
{
  if(enable) {
    thermo_power_on();
  } else {
    thermo_power_off();
  }
}

void thermo_hi_threshold_reset(void)
{
	thermo_hi_threshold_set(thermo_threshold_hi);
}

void thermo_lo_threshold_reset(void)
{
	thermo_lo_threshold_set(thermo_threshold_lo);
}

void thermo_proc(enum task_events ev)
{
  CDBG("thermo_proc\n");
  run_state_machine(ev);
}

void thermo_enter_powersave(void)
{
  CDBG("thermo_enter_powersave\n");
}

void thermo_leave_powersave(void)
{
  CDBG("thermo_leave_powersave\n");
}