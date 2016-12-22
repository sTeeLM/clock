#include "beeper.h"
#include "debug.h"

static enum beeper_music beeper_music_index;

void beeper_initialize (void)
{
  CDBG("beeper_initialize\n");
}

void beeper_enter_powersave(void)
{
  
}

void beeper_leave_powersave(void)
{
  
}

void beeper_beep(void)
{
  CDBG("beeper_beep!\n");
}

void beeper_set_music_index(enum beeper_music index)
{
  if(index >= BEEPER_MUSIC_CNT) index = BEEPER_MUSIC_CNT - 1;
  beeper_music_index = index;
}

enum beeper_music beeper_get_music_index()
{
  return beeper_music_index;
}

void beeper_play_music(void)
{
  CDBG("beeper_play_music\n");
}

void beeper_stop_music(void)
{
  CDBG("beeper_stop_music\n");
}