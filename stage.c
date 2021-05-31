#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "stage.h"
#include "entity.h"
#include "star.h"
#include "explosion.h"
#include "debris.h"

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

#define FPS 60
#define MAX_STARS 500
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define PLAYER_SPEED 4
#define PLAYER_BULLET_SPEED 16
#define ALIEN_BULLET_SPEED 8
#define SIDE_PLAYER 0
#define SIDE_ALIEN 1
#define MAX_LINE_LENGTH 1024

#define GLYPH_HEIGHT 28
#define GLYPH_WIDTH  18

static void init_player(stage *s);
static void fire_bullet(stage *s);
static void fire_alien_bullet(entity *e, stage *s);
static void do_player(stage *s, int *keyboard);
static void do_bullets(stage *s);
static void do_enemies(stage *s);
static void do_fighters(stage *s);
static void clip_player();
static void spawn_enemies(stage *s);
static int collision(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2);
static int bullet_hit_fighter(entity *b, stage *s);
static void calc_slope(int x1, int y1, int x2, int y2, float *dx, float *dy);
static void reset_stage(stage *s);
static void init_starfield();
static void draw_background(SDL_Renderer *r);
static void draw_starfield(SDL_Renderer *r);
static void do_background();
static void do_starfield();
static void do_debris(stage *s);
static void do_explosions(stage *s);
static void draw_debris(stage *s, SDL_Renderer *r);
static void draw_explosions(stage *s, SDL_Renderer *r);
static void add_debris(entity *e, stage *s);
static void add_explosions(int x, int y, int num, stage *s);
static void blit(SDL_Texture *texture, int x, int y, SDL_Renderer *r);
static void blit_rect(SDL_Texture *texture, SDL_Rect *src, int x, int y, SDL_Renderer *r);
static SDL_Texture *load_texture(char *filename, SDL_Renderer *r);
static void draw_text(int x, int y, int r, int g, int b, SDL_Renderer *renderer, char *format, ...);
static void draw_hud(stage *s, SDL_Renderer *renderer);


static SDL_Texture *player_texture;
static SDL_Texture *bullet_texture;
static SDL_Texture *font_texture;
static SDL_Texture *enemy_texture;
static SDL_Texture *alien_bullet_texture;
static SDL_Texture *explosion_texture;
static SDL_Texture *background;

static entity *player;

static int spawn_timer;
static int reset_timer;
static int background_x;
static star stars[MAX_STARS];

static char draw_text_buffer[MAX_LINE_LENGTH];

stage *init_stage(SDL_Renderer *r)
{
	stage *s = calloc(1, sizeof(stage));
	s->fighter_tail = &s->fighter_head;
	s->bullet_tail = &s->bullet_head;
	s->score = 0;

	bullet_texture = load_texture("gfx/playerBullet.png", r);
	enemy_texture = load_texture("gfx/enemy.png", r);
	alien_bullet_texture = load_texture("gfx/alienBullet.png", r);
	player_texture = load_texture("gfx/player.png", r);
	background = load_texture("gfx/background.png", r);
	explosion_texture = load_texture("gfx/explosion.png", r);
	font_texture = load_texture("gfx/font.png", r);
	
	reset_stage(s);

	puts("init_stage");
	return s;
}

static void reset_stage(stage *s)
{
	entity *e;
	explosion *ex;
	debris *d;

	while(s->fighter_head.next) {
		e = s->fighter_head.next;
		s->fighter_head.next = e->next;
		free(e);
	}

	while (s->bullet_head.next) {
		e = s->bullet_head.next;
		s->bullet_head.next = e->next;
		free(e);
	}

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

	memset(s, 0, sizeof(stage));
	s->fighter_tail = &s->fighter_head;
	s->bullet_tail = &s->bullet_head;
	s->explosion_tail = &s->explosion_head;
	s->debris_tail = &s->debris_head;

	init_player(s);
	init_starfield();

	spawn_timer = 0;
	reset_timer = FPS * 3;
	s->score = 0;

	puts("reset_stage");
}

static void init_player(stage *s)
{
	player = create_entity(100, 100, SIDE_PLAYER, player_texture);
	player->dx = 0;
	player->dy = 0;
	player->health = 3;
	s->fighter_tail->next = player;
	s->fighter_tail = player;
	puts("player initialized");
}

static void init_starfield()
{
	int i;
	for(i = 0; i < MAX_STARS; i++) {
		stars[i].x = rand() % SCREEN_WIDTH;
		stars[i].y = rand() % SCREEN_HEIGHT;
		stars[i].speed = 1 + rand() % 8;
	}
}

void do_logic(int *keyboard, stage *s)
{
	do_background();
	do_starfield();
	do_player(s, keyboard); puts("do player");
	do_enemies(s); puts("do enemies");
	do_fighters(s); puts("do fighters");
	do_bullets(s); puts("do bullets");
	spawn_enemies(s); puts("spawn_enemies");
	clip_player(); puts("clip player");
	do_explosions(s);
	do_debris(s);

	if(player == NULL && --reset_timer <= 0)
		reset_stage(s);
	puts("do logic");
}

void draw(stage *s, SDL_Renderer *r)
{
	draw_background(r);
	draw_starfield(r);

	if(player != NULL)
		blit(player->texture, player->x, player->y, r);
	
	entity *b;
	
	for (b = s->bullet_head.next; b != NULL; b = b->next)
		blit(b->texture, b->x, b->y, r);

	entity *e;
	
	for (e = s->fighter_head.next; e != NULL ; e = e->next)
		blit(e->texture, e->x, e->y, r);
	
	draw_debris(s, r);
	draw_explosions(s, r);
	draw_hud(s, r);
	puts("draw");
}

static void draw_background(SDL_Renderer *r)
{
	SDL_Rect dest;
	int x;
	
	for (x = background_x ; x < SCREEN_WIDTH ; x += SCREEN_WIDTH) {
		dest.x = x;
		dest.y = 0;
		dest.w = SCREEN_WIDTH;
		dest.h = SCREEN_HEIGHT;
		
		SDL_RenderCopy(r, background, NULL, &dest);
	}
}

static void draw_starfield(SDL_Renderer *r)
{
	int i, c;
	
	for (i = 0 ; i < MAX_STARS ; i++) {
		c = 32 * stars[i].speed;
		
		SDL_SetRenderDrawColor(r, c, c, c, 255);
		
		SDL_RenderDrawLine(r, stars[i].x, stars[i].y, stars[i].x + 3, stars[i].y);
	}
}

static void draw_debris(stage *s, SDL_Renderer *r)
{
	debris *d;
	
	for (d = s->debris_head.next ; d != NULL ; d = d->next)
		blit_rect(d->texture, &d->rect, d->x, d->y, r);
}

static void draw_explosions(stage *s, SDL_Renderer *r)
{
	explosion *ex;

	SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_ADD);
	SDL_SetTextureBlendMode(explosion_texture, SDL_BLENDMODE_ADD);
	
	for (ex = s->explosion_head.next ; ex != NULL ; ex = ex->next) {
		SDL_SetTextureColorMod(explosion_texture, ex->r, ex->g, ex->b);
		SDL_SetTextureAlphaMod(explosion_texture, ex->a);
		
		blit(explosion_texture, ex->x, ex->y, r);
	}
	
	SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_NONE);
}

static void do_background()
{
	if(--background_x < -SCREEN_WIDTH)
		background_x = 0;
}

static void do_starfield()
{
	int i;

	for(i = 0; i < MAX_STARS; i++) {
		stars[i].x -= stars[i].speed;

		if(stars[i].x < 0)
			stars[i].x += SCREEN_WIDTH;
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

static void do_debris(stage *s)
{
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

static void do_player(stage *s, int *keyboard)
{
	if(player != NULL) {
	
		player->dx = 0;
		player->dy = 0;
		
		if (player->reload > 0)
			player->reload--;
		
		if (keyboard[SDL_SCANCODE_UP])
			player->dy = -PLAYER_SPEED;
		
		if (keyboard[SDL_SCANCODE_DOWN])
			player->dy = PLAYER_SPEED;
		
		if (keyboard[SDL_SCANCODE_LEFT])
			player->dx = -PLAYER_SPEED;
		
		if (keyboard[SDL_SCANCODE_RIGHT])
			player->dx = PLAYER_SPEED;
		
		if (keyboard[SDL_SCANCODE_RCTRL] && player->reload <= 0)
			fire_bullet(s);
		
		player->x += player->dx;
		player->y += player->dy;
	}
	puts("do player");
}

static void fire_bullet(stage *s)
{
	entity *bullet = create_entity(player->x, player->y, SIDE_PLAYER, bullet_texture);
	
	s->bullet_tail->next = bullet;
	s->bullet_tail = bullet;
	
	bullet->dx = PLAYER_BULLET_SPEED;
	
	bullet->y += (player->h / 2) - (bullet->h / 2);
	
	player->reload = 8;

	puts("fire_bullet");
}

static void do_enemies(stage *s)
{
	entity *e;
	for(e = s->fighter_head.next; e != NULL; e = e->next) {
		if(e != player && player != NULL && --e->reload <= 0)
			fire_alien_bullet(e, s);
	}

	puts("do enemies");
}

static void fire_alien_bullet(entity *e, stage *s)
{
	entity *bullet = create_entity(e->x, e->y, e->side, alien_bullet_texture);
	s->bullet_tail->next = bullet;
	s->bullet_tail = bullet;

	bullet->x += (e->w / 2) - (bullet->w / 2);
	bullet->y += (e->h / 2) - (bullet->h / 2);

	calc_slope(player->x + (player->w / 2), player->y + (player->h / 2), e->x, e->y, &bullet->dx, &bullet->dy);

	bullet->dx *= ALIEN_BULLET_SPEED;
	bullet->dy *= ALIEN_BULLET_SPEED;

	bullet->side = SIDE_ALIEN;

	e->reload = (rand() % FPS * 2);

	puts("alien fire");
}

static void do_fighters(stage *s)
{
	entity *e;
	entity *prev = &s->fighter_head;

	for (e = s->fighter_head.next; e != NULL ; e = e->next) {
		
		e->x += e->dx;
		e->y += e->dy;
		
		if(e != player && e->x < -e->w)
			e->health = 0;
		
		if(e->health == 0) {

			if (e == player)
				player = NULL;
			
			if(e == s->fighter_tail)
				s->fighter_tail = prev;
			
			prev->next = e->next;
			free(e);
			e = prev;
		}
		prev = e;
	}

	puts("do fighters");
}

static void spawn_enemies(stage *s)
{
	entity *enemy;
	
	if (--spawn_timer <= 0) {
		enemy = create_entity(SCREEN_WIDTH, rand() % SCREEN_HEIGHT, SIDE_ALIEN, enemy_texture);
		enemy->dx = -(2 + (rand() % 4));
		enemy->reload = FPS * (1 + (rand() % 3));
		spawn_timer = 30 + (rand() % FPS);
		
		s->fighter_tail->next = enemy;
		s->fighter_tail = enemy;
	}

	puts("spawn enemies");
}

static void clip_player()
{
	entity *p = player;
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

	puts("clip player");
}



static void do_bullets(stage *s)
{
	entity *b;
	entity *prev = &s->bullet_head;
		
	for (b = s->bullet_head.next; b != NULL; b = b->next) {
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
	
	puts("do_bullets");
}

static int collision(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2)
{
	puts("collision");
	return (MAX(x1, x2) < MIN(x1 + w1, x2 + w2)) && (MAX(y1, y2) < MIN(y1 + h1, y2 + h2));
}

static int bullet_hit_fighter(entity *b, stage *s)
{
	entity *e;

	for(e = s->fighter_head.next; e != NULL; e = e->next) {
		if(e->side != b->side && collision(b->x, b->y, b->w, b->h, e->x, e->y, e->w, e->h)) {

			puts("bullet_hit_fighter");
			b->health = 0;
			e->health--;

			if(e->health == 0) {
				add_explosions(e->x, e->y, 32, s);
				add_debris(e, s);
				if(e != player)
					s->score++;
			}
			
			return 1;

		}
	}
	puts("bullet_hit_fighter");
	return 0;
}

void calc_slope(int x1, int y1, int x2, int y2, float *dx, float *dy)
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

	puts("calc_slope");
}

static void blit(SDL_Texture *texture, int x, int y, SDL_Renderer *r)
{
	SDL_Rect dest;
	dest.x = x;
	dest.y = y;

	SDL_QueryTexture(texture, NULL, NULL, &dest.w, &dest.h);
	SDL_RenderCopy(r, texture, NULL, &dest);
	puts("blit");
}

static void blit_rect(SDL_Texture *texture, SDL_Rect *src, int x, int y, SDL_Renderer *r)
{
	SDL_Rect dest;
	
	dest.x = x;
	dest.y = y;
	dest.w = src->w;
	dest.h = src->h;
	
	SDL_RenderCopy(r, texture, src, &dest);
	puts("blit rect");
}

static SDL_Texture *load_texture(char *filename, SDL_Renderer *r)
{
	puts("texture loaded");
	return IMG_LoadTexture(r, filename);
}

static void draw_text(int x, int y, int r, int g, int b, SDL_Renderer *renderer, char *format, ...)
{
	int i, len, c;
	SDL_Rect rect;
	va_list args;

	memset(&draw_text_buffer, '\0', sizeof(draw_text_buffer));

	va_start(args, format);
	vsprintf(draw_text_buffer, format, args);
	va_end(args);

	len = strlen(draw_text_buffer);

	rect.w = GLYPH_WIDTH;
	rect.h = GLYPH_HEIGHT;
	rect.y = 0;

	SDL_SetTextureColorMod(font_texture, r, g, b);

	for (i = 0 ; i < len ; i++) {
		c = draw_text_buffer[i];

		if (c >= ' ' && c <= 'Z') {
			rect.x = (c - ' ') * GLYPH_WIDTH;
			blit_rect(font_texture, &rect, x, y, renderer);
			//blit_rect(d->texture, &d->rect, d->x, d->y, r);
			x += GLYPH_WIDTH;
		}
	}
}

static void draw_hud(stage *s, SDL_Renderer *renderer)
{
	draw_text(1080, 10, 255, 255, 255, renderer, "SCORE: %03d", s->score);
	if(player != NULL) {
		draw_text(1080, 50, 255, 255, 255, renderer, "HEALTH: %d", player->health);
	} else {
		draw_text(1080, 50, 255, 255, 255, renderer, "HEALTH: %d", 0);
		draw_text(400, 400, 255, 255, 255, renderer, "YOU GOT DESTROYED!");
	}
}