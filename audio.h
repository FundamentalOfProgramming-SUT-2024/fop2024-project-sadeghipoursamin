#ifndef AUDIO_H
#define AUDIO_H

#include "SDL.h"
#include "SDL_mixer.h"

// Initialize audio system
int init_audio(void);

// Load and play background music
int play_background_music(int track_number);

// Stop currently playing music
void stop_background_music(void);

// Clean up audio system
void cleanup_audio(void);

#endif // AUDIO_H