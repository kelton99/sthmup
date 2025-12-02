#include "sounds.h"
#include <string.h>

static Mix_Chunk *sounds[SND_MAX];
Mix_Music *music;

static void load_sounds(void);

void init_sounds(void)
{
    memset(sounds, 0, sizeof(Mix_Chunk*) * SND_MAX);
    music = NULL;
    load_sounds();
    load_music("sound/background.mp3");
}

static void load_sounds(void)
{
    sounds[SND_PLAYER_FIRE] = Mix_LoadWAV("sound/player_fire.mp3");
    sounds[SND_ALIEN_FIRE] = Mix_LoadWAV("sound/enemy_fire.mp3");
    sounds[SND_PLAYER_DIE] = Mix_LoadWAV("sound/player_die.mp3");
    sounds[SND_PLAYER_HIT] = Mix_LoadWAV("sound/player_hit.mp3");
    sounds[SND_ALIEN_DIE] = Mix_LoadWAV("sound/enemy_die.mp3");
}

void load_music(char *filename)
{
    if(music != NULL) {
        Mix_HaltMusic();
        Mix_FreeMusic(music);
        music = NULL;
    }

    music = Mix_LoadMUS(filename);
}

void play_music(bool loop)
{
    Mix_PlayMusic(music, (loop) ? -1 : 0);
}

void play_sound(int id, int channel)
{
    Mix_PlayChannel(channel, sounds[id], 0);
}