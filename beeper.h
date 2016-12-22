#ifndef __CLOCK_BEEPER_H__
#define __CLOCK_BEEPER_H__

void beeper_initialize (void);
void beeper_enter_powersave(void);
void beeper_leave_powersave(void);

enum beeper_music{
  BEEPER_MUSIC_TWO_TIGER = 0,
  BEEPER_MUSIC_FUCK_ME   = 1,
  BEEPER_MUSIC_CNT
};

void beeper_beep(void);
void beeper_set_music_index(enum beeper_music index);
enum beeper_music beeper_get_music_index();
void beeper_play_music(void);
void beeper_stop_music(void);

#endif
