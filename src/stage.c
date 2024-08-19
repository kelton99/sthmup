#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "GLOBALS.h"
#include "stage.h"
#include "entity.h"
#include "entity_manager.h"
#include "star.h"
#include "explosion.h"
#include "debris.h"
#include "drawer.h"

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

#define FPS 60
#define SIDE_PLAYER 0
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

static int collision(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2);
static void do_bullets(entity_manager *em, stage *s);
static int bullet_hit_fighter(entity *b, stage *s);
static void reset_stage(stage *s);
static void init_starfield(stage *s);
static void do_background();
static void do_starfield(stage *s);
static void do_explosions(stage *s);

static void add_debris(entity *e, stage *s);
static void add_explosions(int x, int y, int num, stage *s);



//DEFINING GLOBAL from GLOBALS.H
int background_x;

void do_debris(stage *s);

//static void draw();

stage *init_stage(SDL_Renderer *r)
{
	stage *s = calloc(1, sizeof(stage));
	s->em = init_entity_manager();
	s->spawn_timer = 0;
	s->reset_timer = FPS * 3;
	s->score = 0;

	init_draw(r);
	
	reset_stage(s);

	return s;
}

static void reset_stage(stage *s)
{

	explosion *ex;
	debris *d;

	em_clean_entities(s->em);

	while (s->explosion_head.next) {
		ex = s->explosion_head.next;
		s->explosion_head.next = ex->next;
		free(ex);
	}

	while(s->debris_head.next) {
		d = s->debris_head.next;
		s->debris_head.next = d->next;
		free(d);
	}

	
	s->em->fighter_tail = &s->em->fighter_head;
	s->em->bullet_tail = &s->em->bullet_head;
	s->explosion_tail = &s->explosion_head;
	s->debris_tail = &s->debris_head;

	em_init_player(s->em);
	init_starfield(s);

	s->spawn_timer = 0;
	s->reset_timer = FPS * 3;
	s->score = 0;
}


static void init_starfield(stage *s)
{
	int i;
	for(i = 0; i < MAX_STARS; i++) {
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
	do_explosions(s);
	do_debris(s);

	if(s->em->player == NULL && --s->reset_timer <= 0)
		reset_stage(s);
}


static void do_background()
{
	if(--background_x < -SCREEN_WIDTH)
		background_x = 0;
}

static void do_starfield(stage *s)
{
	int i;

	for(i = 0; i < MAX_STARS; i++) {
		s->stars[i].x -= s->stars[i].speed;

		if(s->stars[i].x < 0)
			s->stars[i].x += SCREEN_WIDTH;
	}
}

static void do_explosions(stage *s)
{
	explosion *e;
	explosion *prev = &s->explosion_head;

	for(e = s->explosion_head.next; e != NULL; e = e->next) {
		e->x += e->dx;
		e->y += e->dy;

		if(--e->a <= 0) {
			if(e == s->explosion_tail)
				s->explosion_tail = prev;

			prev->next = e->next;
			free(e);
			e = prev;
		}
		prev = e;
	}
}

static void add_explosions(int x, int y, int num, stage *s)
{
	explosion *ex;
	int i;
	for(i = 0; i < num; i++) {
		ex = calloc(1, sizeof(explosion));
		s->explosion_tail->next = ex;
		s->explosion_tail = ex;

		ex->x = x + (rand() % 32) - (rand() % 32);
		ex->y = y + (rand() % 32) - (rand() % 32);
		ex->dx = (rand() % 10) - (rand() % 10);
		ex->dy = (rand() % 10) - (rand() % 10);
		
		ex->dx /= 10;
		ex->dy /= 10;

		switch (rand() % 4) {
			case 0:
				ex->r = 255;
				break;
			case 1:
				ex->r = 255;
				ex->g = 128;
				break;
			case 2:
				ex->r = 255;
				ex->g = 255;
				break;
			default:
				ex->r = 255;
				ex->g = 255;
				ex->b = 255;
				break;
		}
		ex->a = rand() % FPS * 3;
	}
}

static void add_debris(entity *e, stage *s)
{
	debris *d;
	int x, y, w, h;

	w = e->w / 2;
	h = e->h / 2;

	for(y = 0; y <= h; y += h) {
		for(x = 0; x <= w; x += w) {
			d = calloc(1, sizeof(debris));
			s->debris_tail->next = d;
			s->debris_tail = d;
			
			d->x = e->x + e->w / 2;
			d->y = e->y + e->h / 2;

			if(e->side == SIDE_PLAYER) 
				d->dx = (rand() % 9);
			else
				d->dx = -(rand() % 9);

			d->dy = rand() % 3 + (-2) + 1;

			d->life = FPS * 2;
			d->texture = e->texture;
			
			d->rect.x = x;
			d->rect.y = y;
			d->rect.w = w;
			d->rect.h = h;
		}
	}
}

static int collision(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2)
{
	return (MAX(x1, x2) < MIN(x1 + w1, x2 + w2)) && (MAX(y1, y2) < MIN(y1 + h1, y2 + h2));
}

static void do_bullets(entity_manager *em, stage *s)
{
	entity *b;
	entity *prev = &em->bullet_head;
		
	for (b = em->bullet_head.next; b != NULL; b = b->next) {
		b->x += b->dx;
		b->y += b->dy;
		
		if (bullet_hit_fighter(b, s) || b->x < -b->w || b->y < -b->h || b->x > SCREEN_WIDTH || b->y > SCREEN_HEIGHT) {
			if (b == em->bullet_tail)
				em->bullet_tail = prev;
			
			prev->next = b->next;
			free(b);
			b = prev;
		}
		prev = b;
	}
}

static int bullet_hit_fighter(entity *b, stage *s)
{
	entity *e;

	for(e = s->em->fighter_head.next; e != NULL; e = e->next) {
		if(e->side != b->side && collision(b->x, b->y, b->w, b->h, e->x, e->y, e->w, e->h)) {
			b->health = 0;
			e->health--;

			if(e->health == 0) {
				add_explosions(e->x, e->y, 32, s);
				add_debris(e, s);
				if(e != s->em->player)
					s->score++;
			}
			
			return 1;

		}
	}
	return 0;
}

void do_debris(stage *s) {
	debris *d;
	debris *prev = &s->debris_head;

	for (d = s->debris_head.next ; d != NULL ; d = d->next) {
		d->x += d->dx;
		d->y += d->dy;
		
		if (--d->life <= 0) {
			if (d == s->debris_tail)
				s->debris_tail = prev;
			
			prev->next = d->next;
			free(d);
			d = prev;
		}
		prev = d;
	}
}

void draw(stage *s, SDL_Renderer *r)
{
	draw_background(r);
	draw_starfield(s, r);
	entity *player = s->em->player;
	if(player != NULL)
		blit(player->texture, player->x, player->y, r);
	
	entity *b;
	
	for (b = s->em->bullet_head.next; b != NULL; b = b->next)
		blit(b->texture, b->x, b->y, r);

	entity *e;
	
	for (e = s->em->fighter_head.next; e != NULL ; e = e->next)
		blit(e->texture, e->x, e->y, r);
	
	draw_debris(s, r);
	draw_explosions(s, r);
	draw_hud(s, player, r);
}