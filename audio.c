#include "audio.h"
#include <stdio.h>

Mix_Music *current_music = NULL;

int init_audio(void) {
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
        return 0;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
        return 0;
    }

    return 1;
}

int play_background_music(int track_number) {
    stop_background_music();
    const char* music_file;
    switch(track_number) {
        case 0:
            music_file = "/Users/saminsadeghipour/Desktop/project/music/Laura-Branigan-Self-Control-musicfeed.ir_128.mp3";
            break;
        case 1:
            music_file = "/Users/saminsadeghipour/Desktop/project/music/megadeth-peace_sells.mp3";
            break;
        case 2:
            music_file = "/Users/saminsadeghipour/Desktop/project/music/game_of_thrones_main_title.mp3";
            break;
        default:
            music_file = "/Users/saminsadeghipour/Desktop/project/music/Tom Jeri Asli.mp3";
    }

    // Load and play the music
    current_music = Mix_LoadMUS(music_file);
    if (current_music == NULL) {
        printf("Failed to load music! SDL_mixer Error: %s\n", Mix_GetError());
        return 0;
    }

    if (Mix_PlayMusic(current_music, -1) < 0) {
        printf("Failed to play music! SDL_mixer Error: %s\n", Mix_GetError());
        return 0;
    }

    return 1;
}

int play_room_music(RoomTheme theme) {
    stop_background_music();
    const char* music_file;
    
    switch(theme) {
        case ENCHANTED_ROOM:
            music_file = "/Users/saminsadeghipour/Desktop/project/music/Modern Talking – cherry cherry lady.mp3";
            break;
        case TREASURE_ROOM:
            music_file = "/Users/saminsadeghipour/Desktop/project/music/01._holy_wars.the_punishment_due.mp3";
            break;
        case NIGHTMARE_ROOM:
            music_file = "/Users/saminsadeghipour/Desktop/project/music/game_of_thrones_main_title.mp3";
            break;
        default:
            return play_background_music(music_choice);
    }

    current_music = Mix_LoadMUS(music_file);
    if (current_music == NULL) {
        return play_background_music(music_choice); 
    }

    if (Mix_PlayMusic(current_music, -1) < 0) {
        return play_background_music(music_choice);
    }

    return 1;
}

void stop_background_music(void) {
    if (Mix_PlayingMusic()) {
        Mix_HaltMusic();
    }
    if (current_music != NULL) {
        Mix_FreeMusic(current_music);
        current_music = NULL;
    }
}

void cleanup_audio(void) {
    stop_background_music();
    Mix_CloseAudio();
    Mix_Quit();
    SDL_Quit();
}