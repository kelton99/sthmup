#include "drawer.h"
#include "GLOBALS.h"
#include "explosion.h"
#include "debris.h"
#include "highscore.h"
#include "star.h"

#define MAX_LINE_LENGTH 1024
#define GLYPH_HEIGHT 28
#define GLYPH_WIDTH  18
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

static char draw_text_buffer[MAX_LINE_LENGTH];

/*
	DEFINING TEXTURES FROM GLOBALS.H
*/
SDL_Texture *player_texture;
SDL_Texture *bullet_texture;
SDL_Texture *font_texture;
SDL_Texture *enemy_texture;
SDL_Texture *alien_bullet_texture;
SDL_Texture *explosion_texture;
SDL_Texture *score_pod_texture;
SDL_Texture *background;

void init_draw(SDL_Renderer *r)
{
	bullet_texture = IMG_LoadTexture(r, "gfx/playerBullet.png");
	enemy_texture = IMG_LoadTexture(r, "gfx/enemy.png");
	alien_bullet_texture = IMG_LoadTexture(r, "gfx/alienBullet.png");
	player_texture = IMG_LoadTexture(r, "gfx/player.png");
	background = IMG_LoadTexture(r, "gfx/background.png");
	explosion_texture = IMG_LoadTexture(r, "gfx/explosion.png");
	score_pod_texture = IMG_LoadTexture(r, "gfx/points.png");
	font_texture = IMG_LoadTexture(r, "gfx/font.png");
}

void draw_explosions(gfx_manager *gm, SDL_Renderer *r)
{
	SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_ADD);
	SDL_SetTextureBlendMode(explosion_texture, SDL_BLENDMODE_ADD);
	explosion *ex;
	list_for_each_entry(ex, &gm->explosions, list) {
		SDL_SetTextureColorMod(explosion_texture, ex->r, ex->g, ex->b);
		SDL_SetTextureAlphaMod(explosion_texture, ex->a);
		blit(explosion_texture, ex->position.x, ex->position.y, r);
	}
	SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_NONE);
}

void draw_pods(list_head *pods, SDL_Renderer *r)
{
	SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_ADD);
	SDL_SetTextureBlendMode(score_pod_texture, SDL_BLENDMODE_ADD);
	entity *pod;
	list_for_each_entry(pod, pods, list) {
		SDL_SetTextureAlphaMod(score_pod_texture, pod->health * 0.425);
		blit(score_pod_texture, pod->position.x, pod->position.y, r);
	}
	SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_NONE);
}

void draw_debris(gfx_manager *gm, SDL_Renderer *r)
{
	debris *d;
	list_for_each_entry(d, &gm->debris, list) {
		blit_rect(d->texture, &d->rect, d->position.x, d->position.y, r);
	}
}

void draw_background(SDL_Renderer *r)
{
	for (int x = background_x ; x < SCREEN_WIDTH ; x += SCREEN_WIDTH) {
		SDL_Rect dest = {.x = x, .y = 0, .w = SCREEN_WIDTH, .h = SCREEN_HEIGHT};
		SDL_RenderCopy(r, background, NULL, &dest);
	}
}

void draw_starfield(star *stars, SDL_Renderer *r)
{
	for (int i = 0 ; i < MAX_STARS ; i++) {
		int c = 32 * stars[i].speed;
		SDL_SetRenderDrawColor(r, c, c, c, 255);
		
		SDL_RenderDrawLine(r, stars[i].x, stars[i].y, stars[i].x + c / 10, stars[i].y);
	}
}

void draw_text(int x, int y, int r, int g, int b, SDL_Renderer *renderer, char *format, ...)
{
	va_list args;

	memset(&draw_text_buffer, '\0', sizeof(draw_text_buffer));

	va_start(args, format);
	vsprintf(draw_text_buffer, format, args);
	va_end(args);

	int len = strlen(draw_text_buffer);

	SDL_Rect rect = {.w = GLYPH_WIDTH, .h = GLYPH_HEIGHT, .y = 0};

	SDL_SetTextureColorMod(font_texture, r, g, b);

	for (int i = 0 ; i < len ; i++) {
		int c = draw_text_buffer[i];

		if (c >= ' ' && c <= 'Z') {
			rect.x = (c - ' ') * GLYPH_WIDTH;
			blit_rect(font_texture, &rect, x, y, renderer);
			x += GLYPH_WIDTH;
		}
	}
}

void draw_hud(stage *s, entity *player, SDL_Renderer *renderer)
{
	draw_text(1080, 10, 255, 255, 255, renderer, "SCORE: %03d", s->score);
	if(player != NULL) {
		draw_text(1080, 50, 255, 255, 255, renderer, "HEALTH: %d", player->health);
	} else {
		draw_text(1080, 50, 255, 255, 255, renderer, "HEALTH: %d", 0);
		draw_text(400, 400, 255, 255, 255, renderer, "YOU GOT DESTROYED!");
	}
}

void draw_highscore(highscore_table *table, SDL_Renderer *renderer)
{
	draw_text(427, 70, 255, 255, 255, renderer, "Highscores");
	for(int i = 0, y = 150; i < NUM_HIGHSCORES; i++) {
		int blue = 255;
		if(table->highscore[i].recent) {
			blue = 0;
		}
		draw_text(425, y, 255, 255, blue, renderer, "#%d ............. %03d", (i + 1), table->highscore[i].score);
		y += 50;
	}

	draw_text(425, 600, 255, 255, 255, renderer, "PRESS FIRE TO PLAY");
}

void blit(SDL_Texture *texture, int x, int y, SDL_Renderer *r)
{
	SDL_Rect dest = {.x = x, .y = y};
	SDL_QueryTexture(texture, NULL, NULL, &dest.w, &dest.h);
	SDL_RenderCopy(r, texture, NULL, &dest);
}

void blit_rect(SDL_Texture *texture, SDL_Rect *src, int x, int y, SDL_Renderer *r)
{
	SDL_Rect dest = {.x = x, .y = y, .w = src->w, .h = src->h};
	SDL_RenderCopy(r, texture, src, &dest);
}
