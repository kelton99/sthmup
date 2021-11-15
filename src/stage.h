#ifndef __STAGE__
#define __STAGE__

#include <SDL2/SDL.h>
#include "entity.h"
#include "explosion.h"
#include "debris.h"

typedef struct stage {
    entity fighter_head, *fighter_tail;
    entity bullet_head, *bullet_tail;

    explosion explosion_head, *explosion_tail;
    debris debris_head, *debris_tail;
    int score;
} stage;

stage *init_stage(SDL_Renderer *renderer);
void do_logic(int *keyboard, stage *s);
void draw(stage *s, SDL_Renderer *r);
#endif