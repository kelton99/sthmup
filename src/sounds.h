
#ifndef __SOUNDS__
#define __SOUNDS__

#include <SDL2/SDL_mixer.h>
#define SND_MAX 5
#define MAX_SND_CHANNELS 8

enum channels {
    CH_ANY = -1,
    CH_PLAYER,
    CH_ALIEN_FIRE,
};

enum sounds {
    SND_PLAYER_FIRE,
    SND_ALIEN_FIRE,
    SND_PLAYER_HIT,
    SND_PLAYER_DIE,
    SND_ALIEN_DIE,
};

void init_sounds();
void load_music(char *filename);
void play_music(bool loop);
void play_sound(int id, int channel);
#endif