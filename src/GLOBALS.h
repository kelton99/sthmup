
#ifndef __GLOBALS__
#define __GLOBALS__

#include <SDL2/SDL.h>
#include "highscore.h"

extern int background_x;
extern SDL_Texture *player_texture;
extern SDL_Texture *bullet_texture;
extern SDL_Texture *font_texture;
extern SDL_Texture *enemy_texture;
extern SDL_Texture *alien_bullet_texture;
extern SDL_Texture *explosion_texture;
extern SDL_Texture *score_pod_texture;
extern SDL_Texture *background;

extern highscore *new_highscore;
extern int cursor_blink;

#endif
