#include "entity_manager.h"
#include "GLOBALS.h"
#include <SDL2/SDL_scancode.h>

#define SIDE_PLAYER 0
#define SIDE_ALIEN 1
#define PLAYER_SPEED 4
#define PLAYER_BULLET_SPEED 16
#define ALIEN_BULLET_SPEED 8
#define FPS 60
#define MAX(a,b) (((a)>(b))?(a):(b))

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

static void calc_slope(int x1, int y1, int x2, int y2, float *dx, float *dy);

void em_init_player(entity_manager *em)
{
	em->player = create_entity(100, 100, SIDE_PLAYER, player_texture);
	em->player->dx = 0;
	em->player->dy = 0;
	em->player->health = 3;
	em->fighter_tail->next = em->player;
	em->fighter_tail = em->player;
}

entity_manager *init_entity_manager()
{
    entity_manager *em = calloc(1, sizeof(entity_manager));
	em->fighter_tail = &em->fighter_head;
	em->bullet_tail = &em->bullet_head;
	return em;
}

void em_clean_entities(entity_manager *em)
{
    while(em->fighter_head.next) {
		entity *e = em->fighter_head.next;
		em->fighter_head.next = e->next;
		free(e);
	}

	while (em->bullet_head.next) {
		entity *e = em->bullet_head.next;
		em->bullet_head.next = e->next;
		free(e);
	}

}

void em_fire_bullet(entity_manager *em)
{
	entity *bullet = create_entity(em->player->x, em->player->y, SIDE_PLAYER, bullet_texture);
	
	em->bullet_tail->next = bullet;
	em->bullet_tail = bullet;
	
	bullet->dx = PLAYER_BULLET_SPEED;
	
	bullet->y += (em->player->h / 2) - (bullet->h / 2);
	
	em->player->reload = 8;
}

void em_do_player(entity_manager *em, int *keyboard)
{
	if(em->player != NULL) {
	
		em->player->dx = 0;
		em->player->dy = 0;

		if (em->player->reload > 0) {
			em->player->reload--;
		}

		if (keyboard[SDL_SCANCODE_UP]) {
			em->player->dy = -PLAYER_SPEED;
		}

		if (keyboard[SDL_SCANCODE_DOWN]) {
			em->player->dy = PLAYER_SPEED;
		}

		if (keyboard[SDL_SCANCODE_LEFT]) {
			em->player->dx = -PLAYER_SPEED;
		}

		if (keyboard[SDL_SCANCODE_RIGHT]) {
			em->player->dx = PLAYER_SPEED;
		}

		if (keyboard[SDL_SCANCODE_Z] && em->player->reload <= 0) {
			em_fire_bullet(em);
		}

		em->player->x += em->player->dx;
		em->player->y += em->player->dy;
	}
}

void em_do_enemies(entity_manager *em)
{
	entity *e;
	for(e = em->fighter_head.next; e != NULL; e = e->next) {
		if(e != em->player && em->player != NULL && --e->reload <= 0)
			em_fire_alien_bullet(e, em);
	}
}

void em_fire_alien_bullet(entity *e, entity_manager *em)
{
	entity *bullet = create_entity(e->x, e->y, e->side, alien_bullet_texture);
	em->bullet_tail->next = bullet;
	em->bullet_tail = bullet;

	bullet->x += (e->w / 2) - (bullet->w / 2);
	bullet->y += (e->h / 2) - (bullet->h / 2);

	calc_slope(em->player->x + (em->player->w / 2), em->player->y + (em->player->h / 2), e->x, e->y, &bullet->dx, &bullet->dy);

	bullet->dx *= ALIEN_BULLET_SPEED;
	bullet->dy *= ALIEN_BULLET_SPEED;

	bullet->side = SIDE_ALIEN;

	e->reload = (rand() % FPS * 2);
}

void em_do_fighters(entity_manager *em)
{
	entity *e;
	entity *prev = &em->fighter_head;

	for (e = em->fighter_head.next; e != NULL ; e = e->next) {
		
		e->x += e->dx;
		e->y += e->dy;
		
		if(e != em->player && e->x < -e->w)
			e->health = 0;
		
		if(e->health == 0) {

			if (e == em->player)
				em->player = NULL;
			
			if(e == em->fighter_tail)
				em->fighter_tail = prev;
			
			prev->next = e->next;
			free(e);
			e = prev;
		}
		prev = e;
	}
}

void em_spawn_enemies(entity_manager *em, int *spawn_timer)
{
	entity *enemy;
	
	if (--*spawn_timer <= 0) {
		enemy = create_entity(SCREEN_WIDTH, rand() % SCREEN_HEIGHT, SIDE_ALIEN, enemy_texture);
		enemy->dx = -(2 + (rand() % 4));
		enemy->reload = FPS * (1 + (rand() % 3));
		*spawn_timer = 30 + (rand() % FPS);
		
		em->fighter_tail->next = enemy;
		em->fighter_tail = enemy;
	}
}

/*
void em_do_bullets(entity_manager *em)
{
	entity *b;
	entity *prev = &em->bullet_head;
		
	for (b = em->bullet_head.next; b != NULL; b = b->next) {
		b->x += b->dx;
		b->y += b->dy;
		
		if (bullet_hit_fighter(b, s) || b->x < -b->w || b->y < -b->h || b->x > SCREEN_WIDTH || b->y > SCREEN_HEIGHT) {
			if (b == s->bullet_tail)
				s->bullet_tail = prev;
			
			prev->next = b->next;
			free(b);
			b = prev;
		}
		prev = b;
	}
}
*/

void em_clip_player(entity_manager *em)
{
	entity *p = em->player;
	if(p != NULL) {
		if(p->x < 0)
			p->x = 0;

		if(p->y < 0)
			p->y = 0;

		if(p->x > SCREEN_WIDTH / 2)
			p->x = SCREEN_WIDTH / 2;

		if(p->y > SCREEN_HEIGHT - p->h)
			p->y = SCREEN_HEIGHT - p->h;
	}
}

static void calc_slope(int x1, int y1, int x2, int y2, float *dx, float *dy)
{
	int steps = MAX(abs(x1 - x2), abs(y1 - y2));

	if (steps == 0) {
		*dx = *dy = 0;
		return;
	}

	*dx = (x1 - x2);
	*dx /= steps;

	*dy = (y1 - y2);
	*dy /= steps;
}