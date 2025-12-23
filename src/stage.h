#ifndef __STAGE__
#define __STAGE__

#include "gfx_manager.h"
#include "entity_manager.h"
#include "highscore.h"

#define MAX_STARS 500

typedef struct stage {
    entity_manager *em;
    gfx_manager *gm;
    int score;
    int spawn_timer;
    int reset_timer;
} stage;

stage *init_stage();
void do_stage_logic(int *keyboard, stage *s);
void draw_stage(stage *s, SDL_Renderer *r);
void cleanup_stage(stage *s);
#endif