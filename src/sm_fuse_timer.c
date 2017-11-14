#include "sm_fuse_timer.h"
#include "debug.h"
#include "mod_common.h"
#include "led.h"
#include "lt_timer.h"
#include "rom.h"
#include "thermo.h"
#include "gyro.h"
#include "tripwire.h"
#include "hg.h"
#include "fuse.h"
#include "power.h"

#define password_content last_display_s 
#define password_index   lpress_start
#define verify_state     common_state

#define param_step       last_display_s
#define current_temp     lpress_start

const char * code sm_fuse_timer_ss_name[] = 
{
  "SM_FUSE_TIMER_INIT",
  "SM_FUSE_TIMER_PREARMED",
  "SM_FUSE_TIMER_ARMED",
  "SM_FUSE_TIMER_VERIFY",
  "SM_FUSE_TIMER_DISARMED",
  "SM_FUSE_TIMER_PREDETONATE",
  NULL
};

static void display_param_error(unsigned int err)
{
  led_clear();
  led_set_code(5, 'E');
  led_set_code(4, 'R');
  led_set_code(3, 'R');		
  led_set_code(2, (err / 100) + 0x30);
  led_set_code(1, ((err % 100)/10) + 0x30);  
  led_set_code(0, (err % 10) + 0x30); 
}

#define TIMER_PARAM_ERROR 0xFF

enum timer_param_step {
  TIMER_PARAM_GET_CUR_TEMP = 0,
  TIMER_PARAM_CHECK_SET_GYRO,
  TIMER_PARAM_CHECK_SET_HG,
  TIMER_PARAM_CHECK_SET_TRIPWIRE,
  TIMER_PARAM_GET_CHECK_THERMO_HI_LO,
  TIMER_PARAM_CHECK_SET_THERMO_HI,
  TIMER_PARAM_CHECK_SET_THERMO_LO,
  TIMER_PARAM_CHECK_SET_LT_TIME,
  TIMER_PARAM_CHECK_SET_FUSE, 
  TIMER_PARAM_DELAY0,
  TIMER_PARAM_DELAY1,
  TIMER_PARAM_DELAY2,
  TIMER_PARAM_CHECK_CNT
};

enum timer_display_state
{
  DISPLAY_TIMER_DISARMED,
  DISPLAY_TIMER_PREDETONATE,
};

static void display_timer(enum timer_display_state state)
{
  led_clear();
  led_set_code(5, 'P');
  led_set_code(4, 'L');
  led_set_code(3, '0');
  led_set_code(2, '-');
  switch(state) {
    case DISPLAY_TIMER_DISARMED:
      led_set_code(0, 'D');
      led_set_code(1, 'D');
      break;
    case DISPLAY_TIMER_PREDETONATE:
      led_set_code(0, 'C');
      led_set_code(1, 'C');
      break;
  }
}

static void stop_peripheral(void)
{
  tripwire_enable(0);
  thermo_hi_enable(0);
  thermo_lo_enable(0);
  gyro_enable(0);
  hg_enable(0);
}

static void rollback_param(void)
{
  fuse_enable(0);
  stop_peripheral();
  lt_timer_reset();
}

static void display_prearm_step(unsigned char step)
{
  led_clear();
  led_set_code(5, 'P');
  led_set_code(4, 'L');
  led_set_code(3, '0');
  led_set_code(2, '-'); 
  
  led_set_code(1, (TIMER_PARAM_CHECK_CNT - step - 1) / 10 + 0x30);
  led_set_code(0, (TIMER_PARAM_CHECK_CNT - step - 1) % 10 + 0x30);  
}

static bit check_and_set_timer_param(unsigned char step)
{
  unsigned char val, thermo_hi, thermo_lo;

  display_prearm_step(step);
  CDBG("check_and_set_timer_param %bd\n", step);
  
  switch (step) {
    case TIMER_PARAM_GET_CUR_TEMP:
      thermo_hi_enable(1);
      current_temp = (unsigned char)thermo_get_current();
      thermo_hi_enable(0);
      break;
    case TIMER_PARAM_CHECK_SET_GYRO:
      // �������gyro�������Ǻõ�
      val = rom_read(ROM_FUSE_GYRO_ONOFF);
      if(val) {
        val = rom_read(ROM_GYRO_GOOD);
        if(!val) {
          display_param_error(PARAM_ERROR_GYRO_BAD);
          goto err;
        }
        gyro_enable(1);
      }
      break;
    case TIMER_PARAM_CHECK_SET_HG:
      // �������hg�������Ǻõ�
      val = rom_read(ROM_FUSE_HG_ONOFF);
      if(val) {
        val = rom_read(ROM_HG_GOOD);
        if(!val) {
          display_param_error(PARAM_ERROR_HG_BAD);
          goto err;
        }
        hg_enable(1);
      }
      break;
    case TIMER_PARAM_CHECK_SET_TRIPWIRE:
      // �������tripwire�������Ǻõ�
      val = rom_read(ROM_FUSE_TRIPWIRE_ONOFF);
      if(val) {
        val = rom_read(ROM_TRIPWIRE_GOOD);
        if(!val) {
          display_param_error(PARAM_ERROR_TRIPWIRE_BAD);
          goto err;
        }
        tripwire_enable(1);
      }
      break;
    case TIMER_PARAM_GET_CHECK_THERMO_HI_LO:
      // �¶������޲��ܵ���
      thermo_hi = rom_read(ROM_FUSE_THERMO_HI);
      thermo_lo = rom_read(ROM_FUSE_THERMO_LO);
      if(thermo_hi != THERMO_THRESHOLED_INVALID 
        && thermo_lo != THERMO_THRESHOLED_INVALID)
      {
        if((char)thermo_hi <= (char)thermo_lo) {
          display_param_error(PARAM_ERROR_THERMO_HI_LESS_LO);
          goto err;
        }
      }
      break;
    case TIMER_PARAM_CHECK_SET_THERMO_HI:
      // �������thermo hi�������Ǻõ�
      thermo_hi = rom_read(ROM_FUSE_THERMO_HI);
      if(thermo_hi != THERMO_THRESHOLED_INVALID) {
        val = rom_read(ROM_THERMO_HI_GOOD);
        if(!val) {
          display_param_error(PARAM_ERROR_THERMO_HI_BAD);
          goto err;
        }
        if((char)current_temp >= (char)thermo_hi) {
          display_param_error(PARAM_ERROR_THERMO_TOO_HI);
          goto err;
        }
        thermo_hi_enable(1);
        thermo_hi_threshold_set(thermo_hi);
      }
      break;
    case TIMER_PARAM_CHECK_SET_THERMO_LO:
      // �������thermo lo�������Ǻõ�
      thermo_lo = rom_read(ROM_FUSE_THERMO_LO);
      if(thermo_lo != THERMO_THRESHOLED_INVALID) {
        val = rom_read(ROM_THERMO_LO_GOOD);
        if(!val) {
          display_param_error(PARAM_ERROR_THERMO_LO_BAD);
          goto err;
        }
        if((char)current_temp <= (char)thermo_lo) {
          display_param_error(PARAM_ERROR_THERMO_TOO_LOW);
          goto err;
        }
        thermo_lo_enable(1);
        thermo_lo_threshold_set(thermo_lo);
      }
      break;
    case TIMER_PARAM_CHECK_SET_LT_TIME:
      // lt_timerʱ������ڵ�ǰʱ��֮��
      lt_timer_sync_from_rom();
      lt_timer_sync_to_rtc();
      if(!lt_timer_get_relative(1)) {
        display_param_error(PARAM_ERROR_LT_TIMER_OVERFLOW);
        goto err;
      } else {
        lt_timer_start_ram();
      }
      break;
    case TIMER_PARAM_CHECK_SET_FUSE:
      if(rom_read(ROM_FUSE0_SHORT_GOOD) 
        &&rom_read(ROM_FUSE1_SHORT_GOOD)
        &&rom_read(ROM_FUSE0_BROKE_GOOD)
        &&rom_read(ROM_FUSE1_BROKE_GOOD)
      ) {
        fuse_enable(1);
      } else {
        display_param_error(PARAM_ERROR_FUSE_ERROR);
        goto err;
      }
      break;
    case TIMER_PARAM_DELAY0:
    case TIMER_PARAM_DELAY1:
    case TIMER_PARAM_DELAY2:
      break;
  }
  
  return 1;
err:
  return 0;
}

static void verify_password()
{
  unsigned char res;
  
  res = rom_read(ROM_FUSE_PASSWORD + password_index);
  
  verify_state &= ~(1 << password_index);
  
  if(res == password_content) {
    verify_state |= (1 << password_index);
    CDBG("verify_password %bd : %bd <->%bd success\n", password_index, res, password_content);
  } else {
    CDBG("verify_password %bd : %bd <->%bd failed\n", password_index, res, password_content);
  }
  return;
}

static void display_password()
{
  unsigned char i;
  led_clear();
  for( i = 0 ; i < 6 ; i ++ ) {
    if(i < password_index) {
      led_set_code(i, '-');
    } else if(i == password_index) {
      led_set_code(i, password_content + 0x30);
    } else {
      led_set_code(i, 'O');
    }
  }
}

static void inc_password(void)
{
  password_content = (++password_content) % 10;
}

void sm_fuse_timer(unsigned char from, unsigned char to, enum task_events ev)
{
  CDBG("sm_fuse_timer %bd %bd %bd\n", from, to, ev);
  
  // �ӱ��״̬�й���
  if(get_sm_ss_state(to) == SM_FUSE_TIMER_INIT) {
    display_logo(DISPLAY_LOGO_TYPE_FUSE, 3);
    param_step = 0;
    return;
  }
  
  // ��1S����prearm״̬
  if(get_sm_ss_state(from) == SM_FUSE_TIMER_INIT
    && get_sm_ss_state(to) == SM_FUSE_TIMER_PREARMED && ev == EV_1S) {
    display_prearm_step(param_step);
    return;
  }

  // prearm״̬��ÿ1S����һ��������
  if(get_sm_ss_state(from) == SM_FUSE_TIMER_PREARMED
    && get_sm_ss_state(to) == SM_FUSE_TIMER_PREARMED && ev == EV_1S) {
    if(param_step != TIMER_PARAM_ERROR && param_step < TIMER_PARAM_CHECK_CNT) {
      if(!check_and_set_timer_param(param_step)) {
        rollback_param(); // �����˴��󣬿�ʼ�ع�
        param_step = TIMER_PARAM_ERROR;
        set_task(EV_FUSE_SEL0);
      } else {
        param_step ++; //������һ����������
        if(param_step >= TIMER_PARAM_CHECK_CNT) {
          set_task(EV_FUSE_SEL1); // ȫ����ͨ���ˣ�����armed
        }
      }
    }
    return;
  }
  
  // prearm״̬���յ��κ������¼������ع�
  if(get_sm_ss_state(to) == SM_FUSE_TIMER_PREARMED 
    && (ev == EV_COUNTER 
    || ev == EV_FUSE0_SHORT || ev == EV_FUSE0_BROKE 
    || ev == EV_FUSE1_SHORT || ev == EV_FUSE1_BROKE
    || ev == EV_ROTATE_GYRO || ev == EV_DROP_GYRO || ev == EV_ACC_GYRO 
    || ev == EV_ROTATE_HG || ev == EV_TRIPWIRE
    || ev == EV_THERMO_HI || ev == EV_THERMO_LO)) {
    rollback_param(); // �����˴��󣬿�ʼ�ع�
    param_step = TIMER_PARAM_ERROR;
    set_task(EV_FUSE_SEL0);
    return;
  }     
  
  // ����armed״̬����ʼ����ʱ
  if(get_sm_ss_state(from) == SM_FUSE_TIMER_PREARMED 
    && get_sm_ss_state(to) == SM_FUSE_TIMER_ARMED && ev == EV_FUSE_SEL1) {
    lt_timer_display(1);
    password_index   = 0;
    password_content = 0;
    verify_state   = 0;
    power_reset_powersave_to();
    return;
  }
  
  // set0�л���ʾ
  if(get_sm_ss_state(to) == SM_FUSE_TIMER_ARMED && ev == EV_KEY_SET_PRESS) {
    lt_timer_switch_display();
    power_reset_powersave_to();
    return;
  }
  
  if(get_sm_ss_state(to) == SM_FUSE_TIMER_ARMED && ev == EV_KEY_MOD_PRESS) {
    power_reset_powersave_to();
    return;
  }
  
  // mod0����ȴ����������״̬
  if(get_sm_ss_state(from) == SM_FUSE_TIMER_ARMED
    && get_sm_ss_state(to) == SM_FUSE_TIMER_VERIFY 
    && ev == EV_KEY_MOD_PRESS) {
    password_index   = 5;   // current password index
    password_content = 0;   // current displayed password
    verify_state   &= 0xC0;   // verify state�� last 6 bits is 0x3F is success�� 2 bits retry count
    lt_timer_display(0);
    display_password(); // ʹpassword_index��Ϊpassword��index��Ϊ�˼���ramʹ��
    power_reset_powersave_to();
    return;
  } 
    
  // ����������֤ʧ�ܻ���ȡ�������ص���ʱ
  if(get_sm_ss_state(from) == SM_FUSE_TIMER_VERIFY
    && get_sm_ss_state(to) == SM_FUSE_TIMER_ARMED 
    && (ev == EV_FUSE_SEL1 || ev == EV_KEY_SET_LPRESS)) {
    lt_timer_display(1);
    power_reset_powersave_to();
    return;
  }
  
  // ��һ����
  if(get_sm_ss_state(to) == SM_FUSE_TIMER_VERIFY 
    && get_sm_ss_state(from) == SM_FUSE_TIMER_VERIFY 
    && ev == EV_KEY_MOD_PRESS) {
    power_reset_powersave_to();
    if(password_index > 0) {
      verify_password();
      password_content = 0;
      password_index --;
      display_password(); // ʹpassword_index��Ϊpassword��index��Ϊ�˼���ramʹ�á�����
    } else {
      // У���������ͨ��������EV_FUSE_SEL0
      // ���ʧ��С��3�Σ�����EV_FUSE_SEL1
      // ���ʧ�ܴ��ڵ���3�Σ�����EV_FUSE_SEL2
      verify_password();
      if((verify_state & 0x3F) == 0x3F) {
        CDBG("verify pass!\n");
        set_task(EV_FUSE_SEL0); // У��ͨ����
      } else if((verify_state & 0xC0) == 0x80) {
        CDBG("verify failed, to detonate!\n");
        set_task(EV_FUSE_SEL2); // У���β�OK������
      } else {
        verify_state += 0x40;
        CDBG("verify failed, count is %0bx!\n", (verify_state & 0xC0));
        set_task(EV_FUSE_SEL1);
      }
    }
    return;
  }
  
  // ��������
  if(get_sm_ss_state(to) == SM_FUSE_TIMER_VERIFY 
    && get_sm_ss_state(from) == SM_FUSE_TIMER_VERIFY 
    && ev == EV_KEY_SET_PRESS) {
    power_reset_powersave_to();
    inc_password();
    display_password();
  }
  
  // ÿ��̽�����Ƿ����powersave
  if(get_sm_ss_state(from) == SM_FUSE_TIMER_ARMED
    && get_sm_ss_state(to) == SM_FUSE_TIMER_ARMED && ev == EV_1S) {
    power_test_powersave_to();
    return;
  }
  
  // �������
  if(get_sm_ss_state(from) == SM_FUSE_TIMER_VERIFY
    && get_sm_ss_state(to) == SM_FUSE_TIMER_DISARMED && ev == EV_FUSE_SEL0) {
    display_timer(DISPLAY_TIMER_DISARMED);
    rollback_param(); // �ر�������Χ��·���Լ�lt_timer
    set_task(EV_FUSE_SEL0);
    return;
  }
  
  // ��armed/verify ����predetonate״̬
  if(get_sm_ss_state(to) == SM_FUSE_TIMER_PREDETONATE
    &&(get_sm_ss_state(from) == SM_FUSE_TIMER_ARMED || get_sm_ss_state(from) == SM_FUSE_TIMER_VERIFY)
    && (ev == EV_COUNTER || ev == EV_FUSE_SEL2
    || ev == EV_FUSE0_SHORT || ev == EV_FUSE0_BROKE 
    || ev == EV_FUSE1_SHORT || ev == EV_FUSE1_BROKE
    || ev == EV_ROTATE_GYRO || ev == EV_DROP_GYRO || ev == EV_ACC_GYRO 
    || ev == EV_ROTATE_HG || ev == EV_TRIPWIRE
    || ev == EV_THERMO_HI || ev == EV_THERMO_LO)) {
    display_timer(DISPLAY_TIMER_PREDETONATE);
    stop_peripheral(); // �ر�������Χ��·���Լ�lt_timer
    lt_timer_reset();
    set_task(EV_FUSE_SEL0);
    return;
  } 

}