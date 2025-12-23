
#include "stage.h"
#include "drawer.h"
#include "defs.h"
#include "entity_manager.h"
#include "sounds.h"

static void reset_stage(stage *s);

//DEFINING GLOBAL from GLOBALS.H
int background_x;

#define player s->em->player

stage *init_stage()
{
	stage *s = calloc(1, sizeof(stage));
	s->em = init_entity_manager();
	s->gm = init_gfx_manager();
	s->spawn_timer = 0;
	s->reset_timer = FPS * 3;
	s->score = 0;
	init_sounds();
	play_music(true);
	reset_stage(s);
	return s;
}

static void reset_stage(stage *s)
{
	em_clean_entities(s->em);
	gm_clean_gfx(s->gm);
	em_init_player(s->em);
	//init_starfield(s);

	s->spawn_timer = 0;
	s->reset_timer = FPS * 3;
	s->score = 0;
}

void do_stage_logic(int *keyboard, stage *s)
{

	em_do_player(s->em, keyboard);
	em_do_fighters(s->em);
	em_do_bullets(s->em, s->gm);
	em_do_score_pods(s->em, &s->score);
	em_spawn_enemies(s->em, &s->spawn_timer);
	em_clip_player(s->em);
	gm_do_explosions(s->gm);
	gm_do_debris(s->gm);

	if(player == NULL && --s->reset_timer <= 0) {
		reset_stage(s);
	}
}

void draw_stage(stage *s, SDL_Renderer *r)
{
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

	draw_pods(&s->em->score_pods, r);

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