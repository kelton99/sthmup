#ifndef __ENTITY_MANAGER__
#define __ENTITY_MANAGER__

#include <SDL2/SDL.h>
#include "entity.h"
#include "gfx_manager.h"
#include "list.h"

typedef struct entity_manager {
    entity *player;
    list_head fighters;
    list_head bullets;
} entity_manager;

entity_manager *init_entity_manager();
void em_init_player(entity_manager *em);
void em_clean_entities(entity_manager *em);
void em_fire_bullet(entity_manager *em);
void em_do_player(entity_manager *em, int *keyboard);
void em_do_enemies(entity_manager *em);
void em_fire_alien_bullet(entity *e, entity_manager *em);
void em_do_fighters(entity_manager *em);
void em_do_bullets(entity_manager *em, gfx_manager *gm, int *score);
void em_spawn_enemies(entity_manager *em, int *spawn_timer);
void em_clip_player(entity_manager *em);

#endif