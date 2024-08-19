#ifndef __ENTITY_MANAGER__
#define __ENTITY_MANAGER__

#include <SDL2/SDL.h>
#include "entity.h"

typedef struct entity_manager {
    entity *player;
    entity fighter_head, *fighter_tail;
    entity bullet_head, *bullet_tail;
} entity_manager;

entity_manager *init_entity_manager();
void em_init_player(entity_manager *em);
void em_clean_entities(entity_manager *em);
void em_fire_bullet(entity_manager *em);
void em_do_player(entity_manager *em, int *keyboard);
void em_do_enemies(entity_manager *em);
void em_fire_alien_bullet(entity *e, entity_manager *em);
void em_do_fighters(entity_manager *em);
void em_spawn_enemies(entity_manager *em, int *spawn_timer);
//void em_do_bullets(entity_manager *em);
void em_clip_player(entity_manager *em);

#endif