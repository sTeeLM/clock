#ifndef __CLOCK_BEEPER_H__
#define __CLOCK_BEEPER_H__

void beeper_initialize (void);
void beeper_enter_powersave(void);
void beeper_leave_powersave(void);

enum beeper_music{
  BEEPER_MUSIC_BYGHX        = 0, // ���¹���
  BEEPER_MUSIC_ZNPA         = 1, // ף��ƽ��
  BEEPER_MUSIC_SB           = 2, // �ͱ� 
  BEEPER_MUSIC_TWO_TAGER    = 3, // �ͱ�
  BEEPER_MUSIC_CNT
};


void beeper_beep(void);
void beeper_beep_beep(void);
void beeper_beep_beep_always(void);
void beeper_set_beep_enable(bit enable);
bit beeper_get_beep_enable(void);
void beeper_inc_music_index(void);
enum beeper_music beeper_get_music_index();
bit beeper_play_music(void);
void beeper_stop_music(void);

unsigned char beeper_get_music_to(void);
void beeper_inc_music_to(void);


#endif
