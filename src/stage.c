
#include "stage.h"
#include "drawer.h"
#include "defs.h"
#include "sounds.h"

static void reset_stage(stage *s);
static void init_starfield(stage *s);
static void do_background();
static void do_starfield(stage *s);

//DEFINING GLOBAL from GLOBALS.H
int background_x;

#define player s->em->player

stage *init_stage(SDL_Renderer *r)
{
	stage *s = calloc(1, sizeof(stage));
	s->em = init_entity_manager();
	s->gm = init_gfx_manager();
	s->spawn_timer = 0;
	s->reset_timer = FPS * 3;
	s->score = 0;

	init_sounds();
	play_music(true);
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
	em_do_bullets(s->em, s->gm, &s->score);
	em_spawn_enemies(s->em, &s->spawn_timer);
	em_clip_player(s->em);
	gm_do_explosions(s->gm);
	gm_do_debris(s->gm);

	if(player == NULL && --s->reset_timer <= 0) {
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


void draw(stage *s, SDL_Renderer *r)
{
	draw_background(r);
	draw_starfield(s, r);
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