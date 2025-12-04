#include <SDL2/SDL_scancode.h>
#include "entity_manager.h"
#include "GLOBALS.h"
#include "sounds.h"
#include "defs.h"
#include "vec2d.h"

static void calc_slope(int x1, int y1, int x2, int y2, vec2d *velocity);
static void add_score_pods(entity_manager *em, entity *fighter);
static bool collision(entity *bullet, entity *fighter);
static bool is_out_of_bounds(entity *e);
static bool hit(entity *bullet, entity *fighter);

#define player em->player

void em_init_player(entity_manager *em)
{
	player = create_entity(100, 100, SIDE_PLAYER, player_texture);
	player->health = 3;
    list_add_tail(&player->list, &em->fighters);
}

entity_manager *init_entity_manager()
{
    entity_manager *em = calloc(1, sizeof(entity_manager));
	INIT_LIST_HEAD(&em->fighters);
	INIT_LIST_HEAD(&em->bullets);
	INIT_LIST_HEAD(&em->score_pods);
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
	entity *bullet = create_entity(player->position.x, player->position.y, SIDE_PLAYER, bullet_texture);
	list_add_tail(&bullet->list, &em->bullets);
	bullet->velocity.x = PLAYER_BULLET_SPEED;
	bullet->position.y += (player->h / 2) - (bullet->h / 2);
	player->reload = 8;
	play_sound(SND_PLAYER_FIRE, CH_PLAYER);
}

void em_do_player(entity_manager *em, int *keyboard)
{
	if(player != NULL) {

		vec2d_set(&player->velocity, 0, 0);

		if (player->reload > 0) {
			player->reload--;
		}

		if (keyboard[SDL_SCANCODE_UP]) {
			player->velocity.y = -PLAYER_SPEED;
		}

		if (keyboard[SDL_SCANCODE_DOWN]) {
			player->velocity.y = PLAYER_SPEED;
		}

		if (keyboard[SDL_SCANCODE_LEFT]) {
			player->velocity.x = -PLAYER_SPEED;
		}

		if (keyboard[SDL_SCANCODE_RIGHT]) {
			player->velocity.x = PLAYER_SPEED;
		}

		if (keyboard[SDL_SCANCODE_Z] && player->reload <= 0) {
			em_fire_bullet(em);
		}

		//vector normalization
		float magnitude = sqrt(pow(player->velocity.x, 2) + pow(player->velocity.y, 2)) / PLAYER_SPEED;

		if (magnitude > 1) {
			player->velocity.x /= magnitude;
			player->velocity.y /= magnitude;
		}

		vec2d_add(&player->position, &player->velocity);
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
		player->position.x + (player->w / 2),
		player->position.y + (player->h / 2),
		e->position.x, e->position.y,
		&bullet->velocity
	);

	vec2d_scalar(&bullet->velocity, ALIEN_BULLET_SPEED);

	bullet->side = SIDE_ALIEN;

	e->reload = (rand() % FPS * 2);

	float magnitude = sqrt(pow(bullet->velocity.x, 2) + pow(bullet->velocity.y, 2)) / ALIEN_BULLET_SPEED;
	if(magnitude > 1) {
		bullet->velocity.x /= magnitude;
		bullet->velocity.y /= magnitude;
	}
	play_sound(SND_ALIEN_FIRE, CH_ALIEN_FIRE);
}

void em_do_fighters(entity_manager *em)
{
	entity *entry, *temp;
	list_for_each_entry_safe(entry, temp, &em->fighters, list) {
		vec2d_add(&entry->position, &entry->velocity);
		if(entry != player && entry->position.x < -entry->w) {
			entry->health = 0;
		}
		if(entry->health <= 0) {
			if (entry == player) {
				player = NULL;
			}
			list_del(&entry->list);
			free(entry);
		}
		if(entry != player && player != NULL && --entry->reload <= 0) {
			em_fire_alien_bullet(entry, em);
		}
	}
}

void em_do_bullets(entity_manager *em, gfx_manager *gm, int *score)
{
	entity *bullet, *bullet_temp;
	list_for_each_entry_safe(bullet, bullet_temp, &em->bullets, list) {
		vec2d_add(&bullet->position, &bullet->velocity);
		entity *fighter;
		list_for_each_entry(fighter, &em->fighters, list) {
			if(hit(bullet, fighter)) {
				if(fighter->health <= 0) {
					gm_add_explosions(gm, fighter);
					gm_add_debris(gm, fighter);
					if(fighter != player) {
						//++*score;
						add_score_pods(em, fighter);
						play_sound(SND_ALIEN_DIE, CH_ANY);
					} else {
						play_sound(SND_PLAYER_DIE, CH_PLAYER);
					}
				} else {
					play_sound(SND_PLAYER_HIT, CH_ANY);
				}
			}
		}
		if (bullet->health <= 0 || is_out_of_bounds(bullet)) {
			list_del(&bullet->list);
			free(bullet);
		}
	}
}

void em_do_score_pods(entity_manager *em)
{
	entity *pod, *pod_temp;
	list_for_each_entry_safe(pod, pod_temp, &em->score_pods, list) {
		vec2d_add(&pod->position, &pod->velocity);
	}
}

static void add_score_pods(entity_manager *em, entity *fighter)
{
	entity *pod = create_entity(fighter->position.x, fighter->position.y, fighter->side, score_pod_texture);
	INIT_LIST_HEAD(&pod->list);
	list_add_tail(&pod->list, &em->score_pods);
	pod->velocity = create_vec2d(-(rand() % 5), (rand() % 5) - (rand() % 5));
	pod->health = FPS * 10;
}

static bool hit(entity *bullet, entity *fighter)
{
	if(fighter->side != bullet->side && collision(bullet, fighter)) {
		bullet->health--;
		fighter->health--;
		return true;
	}
	return false;
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
	if(player != NULL) {
		if(player->position.x < 0) {
			player->position.x = 0;
		}

		if(player->position.y < 0) {
			player->position.y = 0;
		}

		if(player->position.x > SCREEN_WIDTH / 2) {
			player->position.x = SCREEN_WIDTH / 2;
		}

		if(player->position.y > SCREEN_HEIGHT - player->h) {
			player->position.y = SCREEN_HEIGHT - player->h;
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

static bool collision(entity *bullet, entity *fighter)
{
	return MAX(bullet->position.x, fighter->position.x) < MIN(bullet->position.x + bullet->w, fighter->position.x + fighter->w) && 
	MAX(bullet->position.y, fighter->position.y) < MIN(bullet->position.y + bullet->h, fighter->position.y + fighter->h);
}

static bool is_out_of_bounds(entity *e)
{
	return e->position.x < -e->w || 
	e->position.y < -e->h || 
	e->position.x > SCREEN_WIDTH || 
	e->position.y > SCREEN_HEIGHT;
}