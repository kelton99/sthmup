#ifndef __FX_MANAGER__
#define __FX_MANAGER__

#include "debris.h"
#include "explosion.h"
#include "list.h"

typedef struct gfx_manager {
    list_head explosions;
    list_head debris;
} gfx_manager;

gfx_manager *init_gfx_manager();

void gm_add_debris(gfx_manager *gm, entity *e);
void gm_do_debris(gfx_manager *gm);

void gm_add_explosions(gfx_manager *gm, entity *e);
void gm_do_explosions(gfx_manager *gm);

void gm_clean_gfx(gfx_manager *gm);
#endif