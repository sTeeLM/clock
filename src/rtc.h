#ifndef __CLOCK_RTC_H__
#define __CLOCK_RTC_H__

enum rtc_data_type {
  RTC_TYPE_TIME    = 0, // 时间
  RTC_TYPE_DATE    = 1, // 日期
  RTC_TYPE_ALARM0  = 2, // 闹钟0
  RTC_TYPE_ALARM1  = 3, // 闹钟1
  RTC_TYPE_TEMP    = 4, // 温度
  RTC_TYPE_CTL     = 5,
};

enum rtc_alarm_mode
{
  RTC_ALARM0_MOD_PER_SEC                 = 0,
  RTC_ALARM0_MOD_MATCH_SEC               = 1,  
  RTC_ALARM0_MOD_MATCH_MIN_SEC           = 2, 
  RTC_ALARM0_MOD_MATCH_HOUR_MIN_SEC      = 3, 
  RTC_ALARM0_MOD_MATCH_DATE_HOUR_MIN_SEC = 4,   
  RTC_ALARM0_MOD_MATCH_DAY_HOUR_MIN_SEC  = 5,
  RTC_ALARM0_MOD_CNT                     = 6, 
  RTC_ALARM1_MOD_PER_MIN                 = 7,
  RTC_ALARM1_MOD_MATCH_MIN               = 8,  
  RTC_ALARM1_MOD_MATCH_HOUR_MIN          = 9, 
  RTC_ALARM1_MOD_MATCH_DATE_HOUR_MIN     = 10,   
  RTC_ALARM1_MOD_MATCH_DAY_HOUR_MIN      = 11,     
};

enum rtc_square_rate
{
	RTC_SQUARE_RATE_1HZ    = 0,
	RTC_SQUARE_RATE_1024HZ = 1,
	RTC_SQUARE_RATE_4096HZ = 2,
	RTC_SQUARE_RATE_8192HZ = 3,		
};

void rtc_initialize (void);

bit rtc_is_lt_timer(void);
void rtc_set_lt_timer(bit enable);
void scan_rtc(void);

void rtc_enter_powersave(void);
void rtc_leave_powersave(void);

void rtc_read_data(enum rtc_data_type type);
void rtc_write_data(enum rtc_data_type type);

// 在read_rtc_data(RTC_TYPE_TIME)之后调用
unsigned char rtc_time_get_hour(void);
void rtc_time_set_hour(unsigned char hour);
void rtc_time_set_hour_12(bit enable);
bit rtc_time_get_hour_12(void);
unsigned char rtc_time_get_min(void);
void rtc_time_set_min(unsigned char min);
unsigned char rtc_time_get_sec(void);
void rtc_time_set_sec(unsigned char sec);

// 在rtc_read_data(RTC_TYPE_DATE)之后调用
unsigned char rtc_date_get_year(void);
void rtc_date_set_year(unsigned char year);
unsigned char rtc_date_get_month(void);
void rtc_date_set_month(unsigned char month);
unsigned char rtc_date_get_date(void);
bit rtc_date_set_date(unsigned char date);
unsigned char rtc_date_get_day(void);
void rtc_date_set_day(unsigned char day);

// 在rtc_read_data(RTC_TYPE_ALARM0)或者RTC_TYPE_ALARM1之后调用
bit rtc_alarm_get_hour_12();
void rtc_alarm_set_hour_12(bit enable);
unsigned char rtc_alarm_get_day(void);
void rtc_alarm_set_day(unsigned char day);
unsigned char rtc_alarm_get_date(void);
void rtc_alarm_set_date(unsigned char date);
unsigned char rtc_alarm_get_hour(void);
void rtc_alarm_set_hour(unsigned char hour);
unsigned char rtc_alarm_get_min();
void rtc_alarm_set_min( unsigned char min);
unsigned char rtc_alarm_get_sec();
void rtc_alarm_set_sec( unsigned char sec);
enum rtc_alarm_mode rtc_alarm_get_mod(void);
void rtc_alarm_set_mode(enum rtc_alarm_mode mode);
enum rtc_alarm_mode rtc_alarm_get_mode(void);
const char * rtc_alarm_get_mod_str(void);

// 在rtc_read_data(RTC_TYPE_TEMP)之后调用
bit rtc_get_temperature(unsigned char * integer, unsigned char * flt);

enum rtc_alarm_index {
  RTC_ALARM0 = 0,
  RTC_ALARM1
};

// 在rtc_read_data(RTC_TYPE_CTL)之后调用
void rtc_enable_alarm_int(enum rtc_alarm_index index, bit enable);
bit rtc_test_alarm_int(enum rtc_alarm_index index);
bit rtc_test_alarm_int_flag(enum rtc_alarm_index index);
void rtc_clr_alarm_int_flag(enum rtc_alarm_index index);

// EOSC:
// Enable Oscillator (EOSC). When set to logic 0, the oscillator is started. 
// When set to logic 1, the oscilla- tor is stopped when the DS3231 switches to VBAT. 
// This bit is clear (logic 0) when power is first applied. 
// When the DS3231 is powered by VCC, the oscillator is always on regardless of the status of the EOSC bit. 
// When EOSC is disabled, all register data is static.
bit rtc_test_eosc(void); 
void rtc_set_eosc(bit val);

// BBSQW:
// Battery-Backed Square-Wave Enable (BBSQW). 
// When set to logic 1 and the DS3231 is being powered by the VBAT pin, 
// this bit enables the square- wave or interrupt output when VCC is absent. 
// When BBSQW is logic 0, the INT/SQW pin goes high imped- ance when VCC falls below the power-fail trip point. 
// This bit is disabled (logic 0) when power is first applied.
bit rtc_test_bbsqw(void);
void rtc_set_bbsqw(bit val);

// CONV:
// Convert Temperature (CONV). 
// Setting this bit to 1 forces the temperature sensor to convert the temperature 
// into digital code and execute the TCXO algorithm to update the capacitance array to the oscillator. 
// This can only happen when a conversion is not already in progress.
// The user should check the status bit BSY before forcing the controller to start a new TCXO execution.
// A user-initiated temperature conversion does not affect the internal 64-second update cycle.
// A user-initiated temperature conversion does not affect the BSY bit for approximately 2ms. 
// The CONV bit remains at a 1 from the time it is written until the conver- sion is finished, 
// at which time both CONV and BSY go to 0. 
// The CONV bit should be used when monitoring the status of a user-initiated conversion.
bit rtc_test_conv(void);
void rtc_set_conv(bit val);

// RS:
// Rate Select (RS2 and RS1). These bits control the frequency of the square-wave output when
// the square wave has been enabled. 
// The following table shows the square-wave frequencies that can be select- ed with the RS bits. 
// These bits are both set to logic 1 (8.192kHz) when power is first applied.
enum rtc_square_rate rtc_get_square_rate(void);
void rtc_set_square_rate(enum rtc_square_rate rt);
const char * rtc_get_square_rate_str(void);

// INTCN:
// Interrupt Control (INTCN). 
// This bit controls the INT/SQW signal. 
// When the INTCN bit is set to logic 0, a square wave is output on the INT/SQW pin. 
// When the INTCN bit is set to logic 1, then a match between the timekeeping registers 
// and either of the alarm registers activates the INT/SQW output (if the alarm is also enabled). 
// The corresponding alarm flag is always set regardless of the state of the INTCN bit. 
// The INTCN bit is set to logic 1 when power is first applied.
bit rtc_test_intcn(void);
void rtc_set_intcn(bit val);

// OSF:
// Oscillator Stop Flag (OSF). A logic 1 in this bit indicates that the oscillator 
// either is stopped or was stopped for some period and may be used to judge the 
// validity of the timekeeping data. This bit is set to logic 1 any time that the oscillator stops.
// The following are exam- ples of conditions that can cause the OSF bit to be set:
// 1) The first time power is applied.
// 2) The voltages present on both VCC and VBAT are insufficient to support oscillation.
// 3) The EOSC bit is turned off in battery-backed mode.
// 4) External influences on the crystal (i.e., noise, leakage, etc.).
// This bit remains at logic 1 until written to logic 0.
bit rtc_test_osf(void);
void rtc_set_osf(bit val);

// EN32kHz:
// Enable 32kHz Output (EN32kHz). 
// This bit controls the status of the 32kHz pin. 
// When set to logic 1, the 32kHz pin is enabled and outputs a 32.768kHz squarewave signal. 
// When set to logic 0, the 32kHz pin goes to a high-impedance state. 
// The initial power-up state of this bit is logic 1, and a 32.768kHz square-wave signal 
// appears at the 32kHz pin after a power source is applied to the DS3231 (if the oscillator is running).
bit rtc_test_en32khz(void);
void rtc_set_en32khz(bit val);

// BSY:
// Busy (BSY). This bit indicates the device is busy executing TCXO functions.
// It goes to logic 1 when the conversion signal to the temperature 
// sensor is asserted and then is cleared when the device is in the 1-minute idle state.
bit rtc_test_bsy(void);

void rtc_dump(void);

#endif