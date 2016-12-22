#include "sm.h"
#include "debug.h"
#include "led.h"
#include "beeper.h"

/* state machine */
#include "sm_display.h"
#include "sm_mod_time.h"
#include "sm_mod_alarm.h"
#include "sm_mod_global_flag.h"
#include "sm_pac_hit.h"
#include "sm_counter.h"
#include "sm_timer.h"

// state machine translate defines
// �������ӱ�̬
static const struct sm_trans code sm[] = 
{
  /* SM_DISPLAY */
  // �ӱ��״̬�й�������ֹ�����
  {SM_DISPLAY<<4|SM_DISPLAY_INIT, EV_KEY_MOD_UP, SM_DISPLAY<<4|SM_DISPLAY_HHMMSS, sm_display},  
  // ��mod0��ʾ������
  {SM_DISPLAY<<4|SM_DISPLAY_HHMMSS, EV_KEY_MOD_PRESS, SM_DISPLAY<<4|SM_DISPLAY_YYMMDD, sm_display},
  // ÿ250ms��ȡ��rtc
  {SM_DISPLAY<<4|SM_DISPLAY_HHMMSS, EV_250MS, SM_DISPLAY<<4|SM_DISPLAY_HHMMSS, sm_display},
  // ��mod1�����޸�ʱ��ģʽ
  {SM_DISPLAY<<4|SM_DISPLAY_HHMMSS, EV_KEY_MOD_LPRESS, SM_MODIFY_TIME<<4|SM_MODIFY_TIME_INIT, sm_mod_time},
  // ����0����
  {SM_DISPLAY<<4|SM_DISPLAY_HHMMSS, EV_ALARM0, SM_PAC_HIT<<4|SM_PAC_HIT_ALARM0, sm_pac_hit},
  // ����1����
  {SM_DISPLAY<<4|SM_DISPLAY_HHMMSS, EV_ALARM1, SM_PAC_HIT<<4|SM_PAC_HIT_ALARM1, sm_pac_hit},
  // ����ʱ����
  {SM_DISPLAY<<4|SM_DISPLAY_HHMMSS, EV_COUNTER, SM_PAC_HIT<<4|SM_PAC_HIT_COUNTER, sm_pac_hit},
  // �ý���ڵ�ģʽ��
  {SM_DISPLAY<<4|SM_DISPLAY_HHMMSS, EV_POWER_SAVE, SM_PAC_HIT<<4|SM_PAC_HIT_POWERSAVE, sm_pac_hit},
  // �����ܱ���
  {SM_DISPLAY<<4|SM_DISPLAY_HHMMSS, EV_KEY_SET_LPRESS, SM_TIMER<<4|SM_TIMER_INIT, sm_timer},

  // ��mod0��ʾ���ڼ�
  {SM_DISPLAY<<4|SM_DISPLAY_YYMMDD, EV_KEY_MOD_PRESS, SM_DISPLAY<<4|SM_DISPLAY_WEEK, sm_display},
  // ÿ1s��һ��rtc
  {SM_DISPLAY<<4|SM_DISPLAY_YYMMDD, EV_1S, SM_DISPLAY<<4|SM_DISPLAY_YYMMDD, sm_display},
  // ����0����
  {SM_DISPLAY<<4|SM_DISPLAY_YYMMDD, EV_ALARM0, SM_PAC_HIT<<4|SM_PAC_HIT_ALARM0, sm_pac_hit},
  // ����1����
  {SM_DISPLAY<<4|SM_DISPLAY_YYMMDD, EV_ALARM1, SM_PAC_HIT<<4|SM_PAC_HIT_ALARM1, sm_pac_hit},
  // ����ʱ����
  {SM_DISPLAY<<4|SM_DISPLAY_YYMMDD, EV_COUNTER, SM_PAC_HIT<<4|SM_PAC_HIT_COUNTER, sm_pac_hit},
  // �ý���ڵ�ģʽ��
  {SM_DISPLAY<<4|SM_DISPLAY_YYMMDD, EV_POWER_SAVE, SM_PAC_HIT<<4|SM_PAC_HIT_POWERSAVE, sm_pac_hit},
  // set0�ص�ʱ������ʾģʽ
  {SM_DISPLAY<<4|SM_DISPLAY_YYMMDD, EV_KEY_SET_PRESS, SM_DISPLAY<<4|SM_DISPLAY_HHMMSS, sm_display},
  
  // ��mod0��ʾ�¶�
  {SM_DISPLAY<<4|SM_DISPLAY_WEEK, EV_KEY_MOD_PRESS, SM_DISPLAY<<4|SM_DISPLAY_TEMP, sm_display},
  // ÿ1s��һ��rtc
  {SM_DISPLAY<<4|SM_DISPLAY_WEEK, EV_1S, SM_DISPLAY<<4|SM_DISPLAY_WEEK, sm_display},
  // ����0����
  {SM_DISPLAY<<4|SM_DISPLAY_WEEK, EV_ALARM0, SM_PAC_HIT<<4|SM_PAC_HIT_ALARM0, sm_pac_hit},
  // ����1����
  {SM_DISPLAY<<4|SM_DISPLAY_WEEK, EV_ALARM1, SM_PAC_HIT<<4|SM_PAC_HIT_ALARM1, sm_pac_hit},
  // ����ʱ����
  {SM_DISPLAY<<4|SM_DISPLAY_WEEK, EV_COUNTER, SM_PAC_HIT<<4|SM_PAC_HIT_COUNTER, sm_pac_hit},
  // �ý���ڵ�ģʽ��
  {SM_DISPLAY<<4|SM_DISPLAY_WEEK, EV_POWER_SAVE, SM_PAC_HIT<<4|SM_PAC_HIT_POWERSAVE, sm_pac_hit},
  // set0�ص�ʱ������ʾģʽ
  {SM_DISPLAY<<4|SM_DISPLAY_WEEK, EV_KEY_SET_PRESS, SM_DISPLAY<<4|SM_DISPLAY_HHMMSS, sm_display},
  
  // ��mod0�ص�ʱ������ʾģʽ
  {SM_DISPLAY<<4|SM_DISPLAY_TEMP, EV_KEY_MOD_PRESS, SM_DISPLAY<<4|SM_DISPLAY_HHMMSS, sm_display},
  // ÿ1s��һ��rtc
  {SM_DISPLAY<<4|SM_DISPLAY_TEMP, EV_1S, SM_DISPLAY<<4|SM_DISPLAY_TEMP, sm_display},
  // ����0����
  {SM_DISPLAY<<4|SM_DISPLAY_TEMP, EV_ALARM0, SM_PAC_HIT<<4|SM_PAC_HIT_ALARM0, sm_pac_hit},
  // ����1����
  {SM_DISPLAY<<4|SM_DISPLAY_TEMP, EV_ALARM1, SM_PAC_HIT<<4|SM_PAC_HIT_ALARM1, sm_pac_hit},
  // ����ʱ����
  {SM_DISPLAY<<4|SM_DISPLAY_TEMP, EV_COUNTER, SM_PAC_HIT<<4|SM_PAC_HIT_COUNTER, sm_pac_hit},
  // �ý���ڵ�ģʽ��
  {SM_DISPLAY<<4|SM_DISPLAY_TEMP, EV_POWER_SAVE, SM_PAC_HIT<<4|SM_PAC_HIT_POWERSAVE, sm_pac_hit},
  // set0�ص�ʱ������ʾģʽ
  {SM_DISPLAY<<4|SM_DISPLAY_TEMP, EV_KEY_SET_PRESS, SM_DISPLAY<<4|SM_DISPLAY_HHMMSS, sm_display},

  /* SM_MODIFY_TIME */
  // �ӱ��״̬���룬��ֹ�����
  {SM_MODIFY_TIME<<4|SM_MODIFY_TIME_INIT, EV_KEY_MOD_UP, SM_MODIFY_TIME<<4|SM_MODIFY_TIME_HH, sm_mod_time}, 
  // ��mod0�����޸ķ���ģʽ
  {SM_MODIFY_TIME<<4|SM_MODIFY_TIME_HH, EV_KEY_MOD_PRESS, SM_MODIFY_TIME<<4|SM_MODIFY_TIME_MM, sm_mod_time},
  // ��set0Сʱ++��д��rtc
  {SM_MODIFY_TIME<<4|SM_MODIFY_TIME_HH, EV_KEY_SET_PRESS, SM_MODIFY_TIME<<4|SM_MODIFY_TIME_HH, sm_mod_time},
  // set1Сʱ����++
  {SM_MODIFY_TIME<<4|SM_MODIFY_TIME_HH, EV_KEY_SET_LPRESS, SM_MODIFY_TIME<<4|SM_MODIFY_TIME_HH, sm_mod_time},
  // seţ��ֹͣ++��д��rtc
  {SM_MODIFY_TIME<<4|SM_MODIFY_TIME_HH, EV_KEY_SET_UP, SM_MODIFY_TIME<<4|SM_MODIFY_TIME_HH, sm_mod_time},
  // ÿ250ms��һ��rtc
  {SM_MODIFY_TIME<<4|SM_MODIFY_TIME_HH, EV_250MS, SM_MODIFY_TIME<<4|SM_MODIFY_TIME_HH, sm_mod_time},
  // mod1 �����޸�����ģʽ
  {SM_MODIFY_TIME<<4|SM_MODIFY_TIME_HH, EV_KEY_MOD_LPRESS, SM_MODIFY_ALARM<<4|SM_MODIFY_ALARM_INIT, sm_mod_alarm},
  // mod0 �����޸���ģʽ
  {SM_MODIFY_TIME<<4|SM_MODIFY_TIME_MM, EV_KEY_MOD_PRESS, SM_MODIFY_TIME<<4|SM_MODIFY_TIME_SS, sm_mod_time},
  // set0 ����++��д��rtc
  {SM_MODIFY_TIME<<4|SM_MODIFY_TIME_MM, EV_KEY_SET_PRESS, SM_MODIFY_TIME<<4|SM_MODIFY_TIME_MM, sm_mod_time},
  // set1 ���ӳ���++
  {SM_MODIFY_TIME<<4|SM_MODIFY_TIME_MM, EV_KEY_SET_LPRESS, SM_MODIFY_TIME<<4|SM_MODIFY_TIME_MM, sm_mod_time},
  // seţ��ֹͣ++��д��rtc
  {SM_MODIFY_TIME<<4|SM_MODIFY_TIME_MM, EV_KEY_SET_UP, SM_MODIFY_TIME<<4|SM_MODIFY_TIME_MM, sm_mod_time},
  // ÿ250ms��һ��rtc
  {SM_MODIFY_TIME<<4|SM_MODIFY_TIME_MM, EV_250MS, SM_MODIFY_TIME<<4|SM_MODIFY_TIME_MM, sm_mod_time}, 
  // mod1 �����޸�����ģʽ
  {SM_MODIFY_TIME<<4|SM_MODIFY_TIME_MM, EV_KEY_MOD_LPRESS, SM_MODIFY_ALARM<<4|SM_MODIFY_ALARM_INIT, sm_mod_alarm},
  // mod0�����޸���ģʽ
  {SM_MODIFY_TIME<<4|SM_MODIFY_TIME_SS, EV_KEY_MOD_PRESS, SM_MODIFY_TIME<<4|SM_MODIFY_TIME_YY, sm_mod_time},
  // set0������0��д��rtc
  {SM_MODIFY_TIME<<4|SM_MODIFY_TIME_SS, EV_KEY_SET_PRESS, SM_MODIFY_TIME<<4|SM_MODIFY_TIME_SS, sm_mod_time},
  // ÿ250ms��һ��rtc 
  {SM_MODIFY_TIME<<4|SM_MODIFY_TIME_SS, EV_250MS, SM_MODIFY_TIME<<4|SM_MODIFY_TIME_SS, sm_mod_time},
  // mod1�����޸�����ģʽ
  {SM_MODIFY_TIME<<4|SM_MODIFY_TIME_SS, EV_KEY_MOD_LPRESS, SM_MODIFY_ALARM<<4|SM_MODIFY_ALARM_INIT, sm_mod_alarm},  
  // ��mod0�����޸���ģʽ
  {SM_MODIFY_TIME<<4|SM_MODIFY_TIME_YY, EV_KEY_MOD_PRESS, SM_MODIFY_TIME<<4|SM_MODIFY_TIME_MO, sm_mod_time},
  // set0 ��++
  {SM_MODIFY_TIME<<4|SM_MODIFY_TIME_YY, EV_KEY_SET_PRESS, SM_MODIFY_TIME<<4|SM_MODIFY_TIME_YY, sm_mod_time},
  // ����set�����++
  {SM_MODIFY_TIME<<4|SM_MODIFY_TIME_YY, EV_KEY_SET_LPRESS, SM_MODIFY_TIME<<4|SM_MODIFY_TIME_YY, sm_mod_time},
  // ̧��setֹͣ��++��д��rtc
  {SM_MODIFY_TIME<<4|SM_MODIFY_TIME_YY, EV_KEY_SET_UP, SM_MODIFY_TIME<<4|SM_MODIFY_TIME_YY, sm_mod_time},
  // ÿ250ms��ȡһ��rtc  
  {SM_MODIFY_TIME<<4|SM_MODIFY_TIME_YY, EV_250MS, SM_MODIFY_TIME<<4|SM_MODIFY_TIME_YY, sm_mod_time},
  // mod1 �����޸�����ģʽ  
  {SM_MODIFY_TIME<<4|SM_MODIFY_TIME_YY, EV_KEY_MOD_LPRESS, SM_MODIFY_ALARM<<4|SM_MODIFY_ALARM_INIT, sm_mod_alarm}, 
  // mod0 �����޸���ģʽ
  {SM_MODIFY_TIME<<4|SM_MODIFY_TIME_MO, EV_KEY_MOD_PRESS, SM_MODIFY_TIME<<4|SM_MODIFY_TIME_DD, sm_mod_time},
  // set0 ��++
  {SM_MODIFY_TIME<<4|SM_MODIFY_TIME_MO, EV_KEY_SET_PRESS, SM_MODIFY_TIME<<4|SM_MODIFY_TIME_MO, sm_mod_time},
  // ����set �³���++
  {SM_MODIFY_TIME<<4|SM_MODIFY_TIME_MO, EV_KEY_SET_LPRESS, SM_MODIFY_TIME<<4|SM_MODIFY_TIME_MO, sm_mod_time}, 
  // ̧��setֹͣ��++��д��rtc
  {SM_MODIFY_TIME<<4|SM_MODIFY_TIME_MO, EV_KEY_SET_UP, SM_MODIFY_TIME<<4|SM_MODIFY_TIME_MO, sm_mod_time}, 
  // ÿ250ms��ȡһ��rtc
  {SM_MODIFY_TIME<<4|SM_MODIFY_TIME_MO, EV_250MS, SM_MODIFY_TIME<<4|SM_MODIFY_TIME_MO, sm_mod_time},
  // mod1 �����޸�����ģʽ  
  {SM_MODIFY_TIME<<4|SM_MODIFY_TIME_MO, EV_KEY_MOD_LPRESS, SM_MODIFY_ALARM<<4|SM_MODIFY_ALARM_INIT, sm_mod_alarm},   
  // mod0 �����޸�Сʱģʽ
  {SM_MODIFY_TIME<<4|SM_MODIFY_TIME_DD, EV_KEY_MOD_PRESS, SM_MODIFY_TIME<<4|SM_MODIFY_TIME_HH, sm_mod_time},
  // set0 ��++
  {SM_MODIFY_TIME<<4|SM_MODIFY_TIME_DD, EV_KEY_SET_PRESS, SM_MODIFY_TIME<<4|SM_MODIFY_TIME_DD, sm_mod_time},
  // set1 �ճ���++  
  {SM_MODIFY_TIME<<4|SM_MODIFY_TIME_DD, EV_KEY_SET_LPRESS, SM_MODIFY_TIME<<4|SM_MODIFY_TIME_DD, sm_mod_time},
  // seţ��ֹͣ��++д��rtc  
  {SM_MODIFY_TIME<<4|SM_MODIFY_TIME_DD, EV_KEY_SET_UP, SM_MODIFY_TIME<<4|SM_MODIFY_TIME_DD, sm_mod_time},
  // ÿ250ms��ȡһ��rtc
  {SM_MODIFY_TIME<<4|SM_MODIFY_TIME_DD, EV_250MS, SM_MODIFY_TIME<<4|SM_MODIFY_TIME_DD, sm_mod_time}, 
  // mod1 �����޸�����ģʽ
  {SM_MODIFY_TIME<<4|SM_MODIFY_TIME_DD, EV_KEY_MOD_LPRESS, SM_MODIFY_ALARM<<4|SM_MODIFY_ALARM_INIT, sm_mod_alarm},

  /* SM_MODIFY_ALARM */
  // �ӱ��״̬���룬��ֹ�����
  {SM_MODIFY_ALARM<<4|SM_MODIFY_ALARM_INIT, EV_KEY_MOD_UP, SM_MODIFY_ALARM<<4|SM_MODIFY_ALARM_HH, sm_mod_alarm},
  // set0Сʱ++  
  {SM_MODIFY_ALARM<<4|SM_MODIFY_ALARM_HH, EV_KEY_SET_PRESS, SM_MODIFY_ALARM<<4|SM_MODIFY_ALARM_HH, sm_mod_alarm},
  // set1Сʱ����++
  {SM_MODIFY_ALARM<<4|SM_MODIFY_ALARM_HH, EV_KEY_SET_LPRESS, SM_MODIFY_ALARM<<4|SM_MODIFY_ALARM_HH, sm_mod_alarm},
  // seţ��ֹͣ++��д��rtc
  {SM_MODIFY_ALARM<<4|SM_MODIFY_ALARM_HH, EV_KEY_SET_UP, SM_MODIFY_ALARM<<4|SM_MODIFY_ALARM_HH, sm_mod_alarm},
  // mod0�����޸ķ���ģʽ
  {SM_MODIFY_ALARM<<4|SM_MODIFY_ALARM_HH, EV_KEY_MOD_PRESS, SM_MODIFY_ALARM<<4|SM_MODIFY_ALARM_MM, sm_mod_alarm},
  // mod1�����޸�ȫ��״̬ģʽ
  {SM_MODIFY_ALARM<<4|SM_MODIFY_ALARM_HH, EV_KEY_MOD_LPRESS, SM_MODIFY_GLOBAL_FLAG<<4|SM_MODIFY_GLOBAL_FLAG_INIT, sm_mod_global_flag},
  // ÿ250ms��ȡһ��rtc
  {SM_MODIFY_ALARM<<4|SM_MODIFY_ALARM_HH, EV_250MS, SM_MODIFY_ALARM<<4|SM_MODIFY_ALARM_HH, sm_mod_alarm}, 
  // set0����++
  {SM_MODIFY_ALARM<<4|SM_MODIFY_ALARM_MM, EV_KEY_SET_PRESS, SM_MODIFY_ALARM<<4|SM_MODIFY_ALARM_MM, sm_mod_alarm},
  // set1���ӳ���++
  {SM_MODIFY_ALARM<<4|SM_MODIFY_ALARM_MM, EV_KEY_SET_LPRESS, SM_MODIFY_ALARM<<4|SM_MODIFY_ALARM_MM, sm_mod_alarm}, 
  // seţ��ֹͣ++��д��rtc  
  {SM_MODIFY_ALARM<<4|SM_MODIFY_ALARM_MM, EV_KEY_SET_UP, SM_MODIFY_ALARM<<4|SM_MODIFY_ALARM_MM, sm_mod_alarm}, 
  // mod0����򿪹ر�״̬
  {SM_MODIFY_ALARM<<4|SM_MODIFY_ALARM_MM, EV_KEY_MOD_PRESS, SM_MODIFY_ALARM<<4|SM_MODIFY_ALARM_ON, sm_mod_alarm}, 
  // mod1�����޸�ȫ��״̬ģʽ  
  {SM_MODIFY_ALARM<<4|SM_MODIFY_ALARM_MM, EV_KEY_MOD_LPRESS, SM_MODIFY_GLOBAL_FLAG<<4|SM_MODIFY_GLOBAL_FLAG_INIT, sm_mod_global_flag},
  // ÿ250ms��ȡһ��rtc
  {SM_MODIFY_ALARM<<4|SM_MODIFY_ALARM_MM, EV_250MS, SM_MODIFY_ALARM<<4|SM_MODIFY_ALARM_MM, sm_mod_alarm}, 
  // set0 �����򿪹رգ���д��rtc
  {SM_MODIFY_ALARM<<4|SM_MODIFY_ALARM_ON, EV_KEY_SET_PRESS, SM_MODIFY_ALARM<<4|SM_MODIFY_ALARM_ON, sm_mod_alarm},  
  // mod1�����޸�ȫ��״̬ģʽ  
  {SM_MODIFY_ALARM<<4|SM_MODIFY_ALARM_ON, EV_KEY_MOD_LPRESS, SM_MODIFY_GLOBAL_FLAG<<4|SM_MODIFY_GLOBAL_FLAG_INIT, sm_mod_global_flag}, 
  // ÿ250ms��ȡһ��rtc
  {SM_MODIFY_ALARM<<4|SM_MODIFY_ALARM_ON, EV_250MS, SM_MODIFY_ALARM<<4|SM_MODIFY_ALARM_ON, sm_mod_alarm}, 
  
  /* SM_MODIFY_GLOBAL_FLAG */
  // ��ֹ�����
  {SM_MODIFY_GLOBAL_FLAG<<4|SM_MODIFY_GLOBAL_FLAG_INIT, EV_KEY_MOD_UP, SM_MODIFY_GLOBAL_FLAG<<4|SM_MODIFY_GLOBAL_FLAG_PS, sm_mod_global_flag},
  // set0 ʡ��ģʽ��ʱʱ������
  {SM_MODIFY_GLOBAL_FLAG<<4|SM_MODIFY_GLOBAL_FLAG_PS, EV_KEY_SET_PRESS, SM_MODIFY_GLOBAL_FLAG<<4|SM_MODIFY_GLOBAL_FLAG_PS, sm_mod_global_flag},
  // mod0 �����������㱨ʱon/off״̬
  {SM_MODIFY_GLOBAL_FLAG<<4|SM_MODIFY_GLOBAL_FLAG_PS, EV_KEY_MOD_PRESS, SM_MODIFY_GLOBAL_FLAG<<4|SM_MODIFY_GLOBAL_FLAG_BS, sm_mod_global_flag},  
  // mod1������ʾʱ��״̬
  {SM_MODIFY_GLOBAL_FLAG<<4|SM_MODIFY_GLOBAL_FLAG_PS, EV_KEY_MOD_LPRESS, SM_DISPLAY<<4|SM_DISPLAY_INIT, sm_display},
  // set0 ���㱨ʱon/off
  {SM_MODIFY_GLOBAL_FLAG<<4|SM_MODIFY_GLOBAL_FLAG_BS, EV_KEY_SET_PRESS, SM_MODIFY_GLOBAL_FLAG<<4|SM_MODIFY_GLOBAL_FLAG_BS, sm_mod_global_flag},
  // mod0 ������������ѡ��
  {SM_MODIFY_GLOBAL_FLAG<<4|SM_MODIFY_GLOBAL_FLAG_BS, EV_KEY_MOD_PRESS, SM_MODIFY_GLOBAL_FLAG<<4|SM_MODIFY_GLOBAL_FLAG_ALARM_BEEP, sm_mod_global_flag},
  // set0 ������������
  {SM_MODIFY_GLOBAL_FLAG<<4|SM_MODIFY_GLOBAL_FLAG_ALARM_BEEP, EV_KEY_SET_PRESS, SM_MODIFY_GLOBAL_FLAG<<4|SM_MODIFY_GLOBAL_FLAG_ALARM_BEEP, sm_mod_global_flag},
  // mod0 ����1224Сʱ����״̬
  {SM_MODIFY_GLOBAL_FLAG<<4|SM_MODIFY_GLOBAL_FLAG_ALARM_BEEP, EV_KEY_MOD_PRESS, SM_MODIFY_GLOBAL_FLAG<<4|SM_MODIFY_GLOBAL_FLAG_1224, sm_mod_global_flag}, 
  // mod1������ʾʱ��״̬
  {SM_MODIFY_GLOBAL_FLAG<<4|SM_MODIFY_GLOBAL_FLAG_ALARM_BEEP, EV_KEY_MOD_LPRESS, SM_DISPLAY<<4|SM_DISPLAY_INIT, sm_display}, 
  // set0 1224ģʽ�л�
  {SM_MODIFY_GLOBAL_FLAG<<4|SM_MODIFY_GLOBAL_FLAG_1224, EV_KEY_SET_PRESS, SM_MODIFY_GLOBAL_FLAG<<4|SM_MODIFY_GLOBAL_FLAG_1224, sm_mod_global_flag},
  // mod0 ��������ʡ��ģʽ״̬
  {SM_MODIFY_GLOBAL_FLAG<<4|SM_MODIFY_GLOBAL_FLAG_1224, EV_KEY_MOD_PRESS, SM_MODIFY_GLOBAL_FLAG<<4|SM_MODIFY_GLOBAL_FLAG_PS, sm_mod_global_flag}, 
  // mod1 ������ʾʱ��״̬
  {SM_MODIFY_GLOBAL_FLAG<<4|SM_MODIFY_GLOBAL_FLAG_1224, EV_KEY_MOD_LPRESS, SM_DISPLAY<<4|SM_DISPLAY_HHMMSS, sm_display},

  /* SM_PAC_HIT */
  // mod0�ص�ʱ����ʾģʽ
  {SM_PAC_HIT<<4|SM_PAC_HIT_ALARM0, EV_KEY_MOD_PRESS, SM_DISPLAY<<4|SM_DISPLAY_HHMMSS, sm_display},
  // set0�ص�ʱ����ʾģʽ
  {SM_PAC_HIT<<4|SM_PAC_HIT_ALARM0, EV_KEY_SET_PRESS, SM_DISPLAY<<4|SM_DISPLAY_HHMMSS, sm_display}, 
  // mod0�ص�ʱ����ʾģʽ
  {SM_PAC_HIT<<4|SM_PAC_HIT_ALARM1, EV_KEY_MOD_PRESS, SM_DISPLAY<<4|SM_DISPLAY_HHMMSS, sm_display},
  // set0�ص�ʱ����ʾģʽ
  {SM_PAC_HIT<<4|SM_PAC_HIT_ALARM1, EV_KEY_SET_PRESS, SM_DISPLAY<<4|SM_DISPLAY_HHMMSS, sm_display}, 
  // mod0�ص�ʱ����ʾģʽ
  {SM_PAC_HIT<<4|SM_PAC_HIT_COUNTER, EV_KEY_MOD_PRESS, SM_DISPLAY<<4|SM_DISPLAY_HHMMSS, sm_display},
  // set0�ص�ʱ����ʾģʽ
  {SM_PAC_HIT<<4|SM_PAC_HIT_COUNTER, EV_KEY_SET_PRESS, SM_DISPLAY<<4|SM_DISPLAY_HHMMSS, sm_display},
  // mod0�ص�ʱ����ʾģʽ 
  {SM_PAC_HIT<<4|SM_PAC_HIT_POWERSAVE, EV_KEY_MOD_PRESS, SM_DISPLAY<<4|SM_DISPLAY_HHMMSS, sm_display},
  // set0�ص�ʱ����ʾģʽ
  {SM_PAC_HIT<<4|SM_PAC_HIT_POWERSAVE, EV_KEY_SET_PRESS, SM_DISPLAY<<4|SM_DISPLAY_HHMMSS, sm_display},
  // ����PS��������0��ʾģʽ
  {SM_PAC_HIT<<4|SM_PAC_HIT_POWERSAVE, EV_ALARM0, SM_PAC_HIT<<4|SM_PAC_HIT_ALARM0, sm_pac_hit},  
  // ����PS�������㱨ʱ��ʾģʽ
  {SM_PAC_HIT<<4|SM_PAC_HIT_POWERSAVE, EV_ALARM1, SM_PAC_HIT<<4|SM_PAC_HIT_ALARM1, sm_pac_hit}, 
  // ˢ����ʾ����ʱ
  {SM_PAC_HIT<<4|SM_PAC_HIT_ALARM0, EV_1S, SM_PAC_HIT<<4|SM_PAC_HIT_ALARM0, sm_pac_hit},
  {SM_PAC_HIT<<4|SM_PAC_HIT_ALARM1, EV_1S, SM_PAC_HIT<<4|SM_PAC_HIT_ALARM1, sm_pac_hit},
  {SM_PAC_HIT<<4|SM_PAC_HIT_COUNTER, EV_1S, SM_PAC_HIT<<4|SM_PAC_HIT_COUNTER, sm_pac_hit},
  
  /* SM_TIMER */
  // ��ֹ�����
  {SM_TIMER<<4|SM_TIMER_INIT, EV_KEY_SET_UP, SM_TIMER<<4|SM_TIMER_CLEAR, sm_timer},
  // mod1�����ʱ��ģʽ
  {SM_TIMER<<4|SM_TIMER_CLEAR, EV_KEY_MOD_LPRESS, SM_COUNTER<<4|SM_COUNTER_INIT, sm_counter},
  // mod0�ܱ�ʼ��
  {SM_TIMER<<4|SM_TIMER_CLEAR, EV_KEY_MOD_PRESS, SM_TIMER<<4|SM_TIMER_RUNNING, sm_timer}, 
  // set0�ƴ�
  {SM_TIMER<<4|SM_TIMER_CLEAR, EV_KEY_SET_PRESS, SM_TIMER<<4|SM_TIMER_RUNNING, sm_timer},
  // mod0�ܱ�ֹͣ  
  {SM_TIMER<<4|SM_TIMER_RUNNING, EV_KEY_MOD_PRESS, SM_TIMER<<4|SM_TIMER_STOP, sm_timer},
  // mod0�ܱ�����
  {SM_TIMER<<4|SM_TIMER_STOP, EV_KEY_MOD_PRESS, SM_TIMER<<4|SM_TIMER_CLEAR, sm_timer},
  // set0�����ʾ�ƴ�
  {SM_TIMER<<4|SM_TIMER_STOP, EV_KEY_SET_PRESS, SM_TIMER<<4|SM_TIMER_STOP, sm_timer}, 
  
  /* SM_COUNTER */
  // ��ֹ�����
  {SM_COUNTER<<4|SM_COUNTER_INIT, EV_KEY_MOD_UP, SM_COUNTER<<4|SM_COUNTER_MODIFY_HH, sm_counter},
  // mod0�����޸ķ���״̬
  {SM_COUNTER<<4|SM_COUNTER_MODIFY_HH, EV_KEY_MOD_PRESS, SM_COUNTER<<4|SM_COUNTER_MODIFY_MM, sm_counter},
  // set0Сʱ++
  {SM_COUNTER<<4|SM_COUNTER_MODIFY_HH, EV_KEY_SET_PRESS, SM_COUNTER<<4|SM_COUNTER_MODIFY_HH, sm_counter},
  // set1Сʱ����++
  {SM_COUNTER<<4|SM_COUNTER_MODIFY_HH, EV_KEY_SET_LPRESS, SM_COUNTER<<4|SM_COUNTER_MODIFY_HH, sm_counter}, 
  // seţ��Сʱ����++ֹͣ
  {SM_COUNTER<<4|SM_COUNTER_MODIFY_HH, EV_KEY_SET_UP, SM_COUNTER<<4|SM_COUNTER_MODIFY_HH, sm_counter},   
  // mod1�л�ʱ����ʾ
  {SM_COUNTER<<4|SM_COUNTER_MODIFY_HH, EV_KEY_MOD_LPRESS, SM_DISPLAY<<4|SM_DISPLAY_INIT, sm_display},
  // mod0�����޸���״̬
  {SM_COUNTER<<4|SM_COUNTER_MODIFY_MM, EV_KEY_MOD_PRESS, SM_COUNTER<<4|SM_COUNTER_MODIFY_SS, sm_counter},
  // set0 ����++
  {SM_COUNTER<<4|SM_COUNTER_MODIFY_MM, EV_KEY_SET_PRESS, SM_COUNTER<<4|SM_COUNTER_MODIFY_MM, sm_counter},
  // set1 ���ӿ���++
  {SM_COUNTER<<4|SM_COUNTER_MODIFY_MM, EV_KEY_SET_LPRESS, SM_COUNTER<<4|SM_COUNTER_MODIFY_MM, sm_counter},
  // set ̧����ӿ���++ֹͣ  
  {SM_COUNTER<<4|SM_COUNTER_MODIFY_MM, EV_KEY_SET_UP, SM_COUNTER<<4|SM_COUNTER_MODIFY_MM, sm_counter},
  // mod1 �ص�ʱ����ʾ  
  {SM_COUNTER<<4|SM_COUNTER_MODIFY_MM, EV_KEY_MOD_LPRESS, SM_DISPLAY<<4|SM_DISPLAY_INIT, sm_display},  
  // mod0 ��ʼ����ʱ
  {SM_COUNTER<<4|SM_COUNTER_MODIFY_SS, EV_KEY_MOD_PRESS, SM_COUNTER<<4|SM_COUNTER_RUNNING, sm_counter},
  // set0 ��++
  {SM_COUNTER<<4|SM_COUNTER_MODIFY_SS, EV_KEY_SET_PRESS, SM_COUNTER<<4|SM_COUNTER_MODIFY_SS, sm_counter},
  // set1 �����++
  {SM_COUNTER<<4|SM_COUNTER_MODIFY_SS, EV_KEY_SET_LPRESS, SM_COUNTER<<4|SM_COUNTER_MODIFY_SS, sm_counter},
  // seţ�������++ֹͣ
  {SM_COUNTER<<4|SM_COUNTER_MODIFY_SS, EV_KEY_SET_UP, SM_COUNTER<<4|SM_COUNTER_MODIFY_SS, sm_counter},
  // mod1�ص�ʱ����ʾ  
  {SM_COUNTER<<4|SM_COUNTER_MODIFY_SS, EV_KEY_MOD_LPRESS, SM_DISPLAY<<4|SM_DISPLAY_INIT, sm_display},
  // set0 ��ͣ����ʱ
  {SM_COUNTER<<4|SM_COUNTER_RUNNING, EV_KEY_SET_PRESS, SM_COUNTER<<4|SM_COUNTER_STOP, sm_counter}, 
  // set0 ��������ʱ
  {SM_COUNTER<<4|SM_COUNTER_STOP, EV_KEY_SET_PRESS, SM_COUNTER<<4|SM_COUNTER_RUNNING, sm_counter},
  // mod0 ���
  {SM_COUNTER<<4|SM_COUNTER_STOP, EV_KEY_MOD_PRESS, SM_COUNTER<<4|SM_COUNTER_MODIFY_MM, sm_counter},
  // ����ʱ����
  {SM_COUNTER<<4|SM_COUNTER_RUNNING, EV_COUNTER, SM_PAC_HIT<<4|SM_PAC_HIT_COUNTER, sm_pac_hit},
};

static unsigned char sm_state; // hi 4 bits : state, lo 4 bits: sub-state 

void state_machine_timer_proc(enum task_events ev)
{
  run_state_machine(ev);
}

void run_state_machine(enum task_events ev)
{
  unsigned char c;
  unsigned char newstate;
  for (c = 0 ; c < sizeof(sm)/sizeof(struct sm_trans) ; c++) {
    if(sm_state == sm[c].from_state && ev == sm[c].event) {
      newstate = sm[c].to_state;
      CDBG("SM: %bd %bd %bd|%bd -> %bd|%bd\n", c, ev,
        get_sm_state(sm_state), get_sm_ss_state(sm_state), 
        get_sm_state(sm[c].to_state), get_sm_ss_state(sm[c].to_state));
      sm[c].sm_proc(sm_state, sm[c].to_state, ev);
      sm_state = sm[c].to_state;
      break;
    }
  }
}

void sm_initialize (void) 
{
  CDBG("sm_initialize\n");
  sm_state = SM_DISPLAY|SM_DISPLAY_INIT;
  set_task(EV_KEY_MOD_UP);
}
