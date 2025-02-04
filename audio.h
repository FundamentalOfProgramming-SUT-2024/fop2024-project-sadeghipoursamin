#ifndef AUDIO_H
#define AUDIO_H

#include "SDL.h"
#include "SDL_mixer.h"

#define ROOM_MUSIC_NORMAL 0
#define ROOM_MUSIC_ENCHANTED 3
#define ROOM_MUSIC_TREASURE 4
#define ROOM_MUSIC_NIGHTMARE 5
#include "game.h" 

extern Mix_Music *current_music;
extern int music_choice;

int play_room_music(RoomTheme theme);
int init_audio(void);
int play_background_music(int track_number);
void stop_background_music(void);
void cleanup_audio(void);

#endif