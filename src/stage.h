#ifndef __STAGE__
#define __STAGE__

#include <SDL2/SDL.h>
#include "entity.h"
#include "explosion.h"
#include "debris.h"
#include "star.h"
#include "entity_manager.h"

#define MAX_STARS 500

typedef struct stage {
    entity_manager *em;
    explosion explosion_head, *explosion_tail;
    debris debris_head, *debris_tail;
    star stars[MAX_STARS];
    int score;
    int spawn_timer;
    int reset_timer;
} stage;

stage *init_stage(SDL_Renderer *renderer);
void do_logic(int *keyboard, stage *s);
void draw(stage *s, SDL_Renderer *r);
#endif