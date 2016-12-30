#ifndef __CLOCK_BEEPER_H__
#define __CLOCK_BEEPER_H__

void beeper_initialize (void);
void beeper_enter_powersave(void);
void beeper_leave_powersave(void);

enum beeper_music{
  BEEPER_MUSIC_BYGHX        = 0, // 八月桂花香
  BEEPER_MUSIC_ZNPA         = 1, // 祝你平安
  BEEPER_MUSIC_SB           = 2, // 送别 
  BEEPER_MUSIC_TWO_TAGER    = 3, // 送别
  BEEPER_MUSIC_CNT
};

enum beeper_music_timeout {
  BEEPER_MUSIC_TO_30        = 0,
  BEEPER_MUSIC_TO_60        = 1,
  BEEPER_MUSIC_TO_90        = 2,
  BEEPER_MUSIC_TO_CNT
};

void beeper_beep(void);
void beeper_beep_beep(void);
void beeper_beep_beep_always(void);
void beeper_set_beep_enable(bit enable);
bit beeper_get_beep_enable(void);
void beeper_inc_music_index(void);
enum beeper_music beeper_get_music_index();
void beeper_play_music(void);
void beeper_stop_music(void);

enum beeper_music_timeout beeper_get_music_to(void);
unsigned char beeper_get_music_to_s(void);
void beeper_inc_music_to(void);


#endif
