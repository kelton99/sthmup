#ifndef __FX_MANAGER__
#define __FX_MANAGER__

#include "debris.h"
#include "explosion.h"

typedef struct gfx_manager {
    explosion explosion_head, *explosion_tail;
    debris debris_head, *debris_tail;
} gfx_manager;

gfx_manager *init_gfx_manager();

void gm_add_debris(gfx_manager *gm, entity *e);
void gm_do_debris(gfx_manager *gm);

void gm_add_explosions(gfx_manager *gm, entity *e);
void gm_do_explosions(gfx_manager *gm);

void gm_clean_gfx(gfx_manager *gm);
#endif