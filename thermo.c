#include "thermo.h"
#include "debug.h"
#include "task.h"
#include "serial_hub.h"
#include "sm.h"
#include "i2c.h"
#include "misc.h"

#define THERMO_HI_I2C_ADDRESS  0x90 //1001 0000
#define THERMO_LO_I2C_ADDRESS  0x92 //1001 0010
#define THERMO_HUB_I2C_ADDRESS  0x48 //0100 1000

#define THERMO_THRESHOLED_MAX (85)
#define THERMO_THRESHOLED_MIN (-55)

static bit thermo_hi_is_enable;
static bit thermo_lo_is_enable;

void thermo_initialize (void)
{
  unsigned int val;
  CDBG("thermo_initialize\n");
  // stop thermo T
  I2C_Put(THERMO_HI_I2C_ADDRESS, 0x22, 0);
  I2C_Put(THERMO_LO_I2C_ADDRESS, 0x22, 0);
  
  // DONE|THF|TLF|NVB|1|0|POL|1SHOT
  // 0|0|0|0|0|0|0|0
  // set thermo0 config
  I2C_Put(THERMO_HI_I2C_ADDRESS, 0xAC, 0x00);
  // 0|0|0|0|0|0|1|0
  // set thermo1 config
  I2C_Put(THERMO_LO_I2C_ADDRESS, 0xAC, 0x02);  
  
  // set th and tl
  thermo_hi_threshold_reset(); 
  thermo_lo_threshold_reset();
  
  // Configuration Register 设置为全1，用于input
  I2C_Put(THERMO_HUB_I2C_ADDRESS, 0x3, 0xFF);
  
  // Polarity Inversion Register 设置为全0
  I2C_Put(THERMO_HUB_I2C_ADDRESS, 0x2, 0x0);

  // 开启测温
  I2C_Put(THERMO_HI_I2C_ADDRESS, 0xEE, 0);
  I2C_Put(THERMO_LO_I2C_ADDRESS, 0xEE, 0);

  delay_ms(10); // delay 10 ms

  // 读取一次端口寄存器消除中断
  I2C_Get(THERMO_HUB_I2C_ADDRESS, 0x0, &val);
  CDBG("thermo int reg is %bx\n", val);

}

void scan_thermo(void)
{
  unsigned char val;
  CDBG("scan_thermo\n");
  
  // 读取一次端口寄存器消除中断
  I2C_Get(THERMO_HUB_I2C_ADDRESS, 0x0, &val);
  CDBG("thermo int reg is %bx\n", val);
  
  if((val & 0x1) == 0 && thermo_hi_is_enable) {
    CDBG("EV_THERMO_HI!\n");
    set_task(EV_THERMO_HI);
  } else if((val & 0x2) == 0 && thermo_lo_is_enable) {
    CDBG("EV_THERMO_LO!\n");
    set_task(EV_THERMO_LO);
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
  thermo_hi_is_enable = enable;
  if(thermo_hi_is_enable || thermo_lo_is_enable)
    serial_set_ctl_bit(SERIAL_BIT_THERMO_EN, 1);
  else
    serial_set_ctl_bit(SERIAL_BIT_THERMO_EN, 0);
	serial_ctl_out();
}

void thermo_lo_enable(bit enable)
{
	CDBG("thermo_lo_enable %bd\n", enable ? 1 : 0);
  thermo_lo_is_enable = enable;
  if(thermo_hi_is_enable || thermo_lo_is_enable)
    serial_set_ctl_bit(SERIAL_BIT_THERMO_EN, 1);
  else
    serial_set_ctl_bit(SERIAL_BIT_THERMO_EN, 0);
	serial_ctl_out();
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
  I2C_Gets(THERMO_HI_I2C_ADDRESS, 0xA1, 2, &val);
  return thermo_hex_to_temp(val);
}

void thermo_hi_threshold_set(char temp)
{
  unsigned int val;
  CDBG("thermo_hi_threshold_set %bd\n", temp);
  val = thermo_temp_to_hex(temp);
  I2C_Puts(THERMO_HI_I2C_ADDRESS, 0xA1, 2, &val);
  val = thermo_temp_to_hex(temp);
  I2C_Puts(THERMO_HI_I2C_ADDRESS, 0xA2, 2, &val);
}

void thermo_lo_threshold_set(char temp)
{
  unsigned int val;
  CDBG("thermo_lo_threshold_set %bd\n", temp);
  val = thermo_temp_to_hex(temp);
  I2C_Puts(THERMO_LO_I2C_ADDRESS, 0xA1, 2, &val);
  val = thermo_temp_to_hex(temp);
  I2C_Puts(THERMO_LO_I2C_ADDRESS, 0xA2, 2, &val);
}

char thermo_lo_threshold_get()
{
  unsigned int val;
  I2C_Gets(THERMO_LO_I2C_ADDRESS, 0xA1, 2, &val);
  return thermo_hex_to_temp(val);
}

void thermo_hi_threshold_dec()
{
  char val;
  val = thermo_hi_threshold_get();
  if(val > THERMO_THRESHOLED_MIN) {
    val -= 5;
    thermo_hi_threshold_set(val);
  }
}

void thermo_hi_threshold_inc()
{
  char val;
  val = thermo_hi_threshold_get();
  if(val < THERMO_THRESHOLED_MAX) {
    val += 5;
    thermo_hi_threshold_set(val);
  }
}

void thermo_lo_threshold_dec()
{
  char val;
  val = thermo_lo_threshold_get();
  if(val > THERMO_THRESHOLED_MIN) {
    val -= 5;
    thermo_lo_threshold_set(val);
  }
}

void thermo_lo_threshold_inc()
{
  char val;
  val = thermo_lo_threshold_get();
  if(val < THERMO_THRESHOLED_MAX) {
    val += 5;
    thermo_lo_threshold_set(val);
  }
}
