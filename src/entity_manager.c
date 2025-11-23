#include <SDL2/SDL_scancode.h>
#include "entity_manager.h"
#include "GLOBALS.h"
#include "entity.h"
#include "list.h"
#include "vec2d.h"

#define SIDE_PLAYER 0
#define SIDE_ALIEN 1
#define PLAYER_SPEED 4
#define PLAYER_BULLET_SPEED 16
#define ALIEN_BULLET_SPEED 8
#define FPS 60
#define MAX(a,b) (((a)>(b))?(a):(b))

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

static void calc_slope(int x1, int y1, int x2, int y2, vec2d *velocity);

void em_init_player(entity_manager *em)
{
	em->player = create_entity(100, 100, SIDE_PLAYER, player_texture);
	em->player->health = 3;
    list_add_tail(&em->player->list, &em->fighters);
}

entity_manager *init_entity_manager()
{
    entity_manager *em = calloc(1, sizeof(entity_manager));
	INIT_LIST_HEAD(&em->fighters);
	INIT_LIST_HEAD(&em->bullets);
	return em;
}

void em_clean_entities(entity_manager *em)
{
	entity *entry, *temp;
	list_for_each_entry_safe(entry, temp, &em->fighters, list) {
		list_del(&entry->list);
		free(entry);
	}
	
	list_for_each_entry_safe(entry, temp, &em->bullets, list) {
		list_del(&entry->list);
		free(entry);
	}
}

void em_fire_bullet(entity_manager *em)
{
	entity *bullet = create_entity(em->player->position.x, em->player->position.y, SIDE_PLAYER, bullet_texture);
	list_add_tail(&bullet->list, &em->bullets);
	bullet->velocity.x = PLAYER_BULLET_SPEED;
	bullet->position.y += (em->player->h / 2) - (bullet->h / 2);
	em->player->reload = 8;
}

void em_do_player(entity_manager *em, int *keyboard)
{
	if(em->player != NULL) {

		vec2d_set(&em->player->velocity, 0, 0);

		if (em->player->reload > 0) {
			em->player->reload--;
		}

		if (keyboard[SDL_SCANCODE_UP]) {
			em->player->velocity.y = -PLAYER_SPEED;
		}

		if (keyboard[SDL_SCANCODE_DOWN]) {
			em->player->velocity.y = PLAYER_SPEED;
		}

		if (keyboard[SDL_SCANCODE_LEFT]) {
			em->player->velocity.x = -PLAYER_SPEED;
		}

		if (keyboard[SDL_SCANCODE_RIGHT]) {
			em->player->velocity.x = PLAYER_SPEED;
		}

		if (keyboard[SDL_SCANCODE_Z] && em->player->reload <= 0) {
			em_fire_bullet(em);
		}

		//vector normalization
		float magnitude = sqrt(pow(em->player->velocity.x, 2) + pow(em->player->velocity.y, 2)) / 4;

		if (magnitude > 1) {
			em->player->velocity.x /= magnitude;
			em->player->velocity.y /= magnitude;
		}

		vec2d_add(&em->player->position, &em->player->velocity);
	}
}

void em_do_enemies(entity_manager *em)
{
	entity *entry;
	list_for_each_entry(entry, &em->fighters, list) {
		if(entry != em->player && em->player != NULL && --entry->reload <= 0) {
			em_fire_alien_bullet(entry, em);
		}
	}
}

void em_fire_alien_bullet(entity *e, entity_manager *em)
{
	entity *bullet = create_entity(e->position.x, e->position.y, e->side, alien_bullet_texture);
	INIT_LIST_HEAD(&bullet->list);
	list_add_tail(&bullet->list, &em->bullets);

	bullet->position.x += (e->w / 2) - (bullet->w / 2);
	bullet->position.y += (e->h / 2) - (bullet->h / 2);

	calc_slope(
	em->player->position.x + (em->player->w / 2),
	em->player->position.y + (em->player->h / 2),
	e->position.x, e->position.y,
	&bullet->velocity
	);

	vec2d_scalar(&bullet->velocity, ALIEN_BULLET_SPEED);

	bullet->side = SIDE_ALIEN;

	e->reload = (rand() % FPS * 2);
}

void em_do_fighters(entity_manager *em)
{
	entity *entry, *temp;
	list_for_each_entry_safe(entry, temp, &em->fighters, list) {
		vec2d_add(&entry->position, &entry->velocity);
		if(entry != em->player && entry->position.x < -entry->w) {
			entry->health = 0;
		}
		if(entry->health <= 0) {
			if (entry == em->player) {
				em->player = NULL;
			}
			list_del(&entry->list);
			free(entry);
		}
	}
}

void em_spawn_enemies(entity_manager *em, int *spawn_timer)
{
	if (--*spawn_timer <= 0) {
		entity *enemy = create_entity(SCREEN_WIDTH, rand() % SCREEN_HEIGHT, SIDE_ALIEN, enemy_texture);
		INIT_LIST_HEAD(&enemy->list);
		list_add_tail(&enemy->list, &em->fighters);
		enemy->velocity.x = -(2 + (rand() % 4));
		enemy->reload = FPS * (1 + (rand() % 3));
		*spawn_timer = 30 + (rand() % FPS);
	}
}

void em_clip_player(entity_manager *em)
{
	entity *p = em->player;
	if(p != NULL) {
		if(p->position.x < 0) {
			p->position.x = 0;
		}

		if(p->position.y < 0) {
			p->position.y = 0;
		}

		if(p->position.x > SCREEN_WIDTH / 2) {
			p->position.x = SCREEN_WIDTH / 2;
		}

		if(p->position.y > SCREEN_HEIGHT - p->h) {
			p->position.y = SCREEN_HEIGHT - p->h;
		}
	}
}

static void calc_slope(int x1, int y1, int x2, int y2, vec2d *velocity)
{
	int steps = MAX(abs(x1 - x2), abs(y1 - y2));

	if (steps == 0) {
		vec2d_set(velocity, 0, 0);
		return;
	}

	vec2d_set(velocity, (x1 - x2), (y1 - y2));
	velocity->x /= steps;
	velocity->y /= steps;
}
