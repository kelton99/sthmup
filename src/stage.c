#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "GLOBALS.h"
#include "stage.h"
#include "entity.h"
#include "entity_manager.h"
#include "gfx_manager.h"
#include "star.h"
#include "drawer.h"
#include "defs.h"
#include "vec2d.h"
#include "list.h"

static int collision(entity *bullet, entity *fighter);
static void do_bullets(entity_manager *em, stage *s);
static bool bullet_hit_fighter(entity *b, stage *s);
static void reset_stage(stage *s);
static void init_starfield(stage *s);
static void do_background();
static void do_starfield(stage *s);

//DEFINING GLOBAL from GLOBALS.H
int background_x;

stage *init_stage(SDL_Renderer *r)
{
	stage *s = calloc(1, sizeof(stage));
	s->em = init_entity_manager();
	s->gm = init_gfx_manager();
	s->spawn_timer = 0;
	s->reset_timer = FPS * 3;
	s->score = 0;

	init_draw(r);
	reset_stage(s);
	return s;
}

static void reset_stage(stage *s)
{
	em_clean_entities(s->em);
	gm_clean_gfx(s->gm);
	em_init_player(s->em);
	init_starfield(s);

	s->spawn_timer = 0;
	s->reset_timer = FPS * 3;
	s->score = 0;
}


static void init_starfield(stage *s)
{
	for(int i = 0; i < MAX_STARS; i++) {
		s->stars[i].x = rand() % SCREEN_WIDTH;
		s->stars[i].y = rand() % SCREEN_HEIGHT;
		s->stars[i].speed = 1 + rand() % 8;
	}
}

void do_logic(int *keyboard, stage *s)
{
	do_background();
	do_starfield(s);
	em_do_player(s->em, keyboard);
	em_do_enemies(s->em);
	em_do_fighters(s->em);
	do_bullets(s->em, s);
	em_spawn_enemies(s->em, &s->spawn_timer);
	em_clip_player(s->em);
	gm_do_explosions(s->gm);
	gm_do_debris(s->gm);

	if(s->em->player == NULL && --s->reset_timer <= 0) {
		reset_stage(s);
	}
}

static void do_background()
{
	if(--background_x < -SCREEN_WIDTH) {
		background_x = 0;
	}
}

static void do_starfield(stage *s)
{
	for(int i = 0; i < MAX_STARS; i++) {
		s->stars[i].x -= s->stars[i].speed;

		if(s->stars[i].x < 0) {
			s->stars[i].x += SCREEN_WIDTH;
		}
	}
}

static int collision(entity *bullet, entity *fighter)
{
	return MAX(bullet->position.x, fighter->position.x) < MIN(bullet->position.x + bullet->w, fighter->position.x + fighter->w) && 
	MAX(bullet->position.y, fighter->position.y) < MIN(bullet->position.y + bullet->h, fighter->position.y + fighter->h);
}

static void do_bullets(entity_manager *em, stage *s)
{
	entity *b, *temp;
	list_for_each_entry_safe(b, temp, &em->bullets, list) {
		vec2d_add(&b->position, &b->velocity);
		if (bullet_hit_fighter(b, s) || b->position.x < -b->w || b->position.y < -b->h || b->position.x > SCREEN_WIDTH || b->position.y > SCREEN_HEIGHT) {
			list_del(&b->list);
			free(b);
		}
	}
}

static bool bullet_hit_fighter(entity *b, stage *s)
{
	entity *e;
	list_for_each_entry(e, &s->em->fighters, list) {
		if(e->side != b->side && collision(b, e)) {
			b->health = 0;
			e->health--;

			if(e->health == 0) {
				gm_add_explosions(s->gm, e);
				gm_add_debris(s->gm, e);
				if(e != s->em->player) {
					s->score++;
				}
			}
			return true;
		}
	}
	return false;
}

void draw(stage *s, SDL_Renderer *r)
{
	draw_background(r);
	draw_starfield(s, r);
	entity *player = s->em->player;
	if(player != NULL) {
		blit(player->texture, player->position.x, player->position.y, r);
	}
	
	entity *b;
	list_for_each_entry(b, &s->em->bullets, list) {
		blit(b->texture, b->position.x, b->position.y, r);
	}

	entity *e;
	list_for_each_entry(e, &s->em->fighters, list) {
		blit(e->texture, e->position.x, e->position.y, r);
	}

	draw_debris(s->gm, r);
	draw_explosions(s->gm, r);
	draw_hud(s, player, r);
}

void cleanup_stage(stage *s)
{
	em_clean_entities(s->em);
	gm_clean_gfx(s->gm);
	free(s->em);
	free(s);
}