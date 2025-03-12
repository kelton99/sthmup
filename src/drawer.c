#include "drawer.h"
#include "GLOBALS.h"
#include "gfx_manager.h"

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
SDL_Texture *background;

void init_draw(SDL_Renderer *r)
{
	bullet_texture = IMG_LoadTexture(r, "gfx/playerBullet.png");
	enemy_texture = IMG_LoadTexture(r, "gfx/enemy.png");
	alien_bullet_texture = IMG_LoadTexture(r, "gfx/alienBullet.png");
	player_texture = IMG_LoadTexture(r, "gfx/player.png");
	background = IMG_LoadTexture(r, "gfx/background.png");
	explosion_texture = IMG_LoadTexture(r, "gfx/explosion.png");
	font_texture = IMG_LoadTexture(r, "gfx/font.png");
}

void draw_explosions(gfx_manager *gm, SDL_Renderer *r)
{
	SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_ADD);
	SDL_SetTextureBlendMode(explosion_texture, SDL_BLENDMODE_ADD);
	
	for (explosion *ex = gm->explosion_head.next ; ex != NULL ; ex = ex->next) {
		SDL_SetTextureColorMod(explosion_texture, ex->r, ex->g, ex->b);
		SDL_SetTextureAlphaMod(explosion_texture, ex->a);
		blit(explosion_texture, ex->x, ex->y, r);
	}
	SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_NONE);
}

void draw_debris(gfx_manager *gm, SDL_Renderer *r)
{
	for (debris *d = gm->debris_head.next ; d != NULL ; d = d->next) {
		blit_rect(d->texture, &d->rect, d->x, d->y, r);
	}
}

void draw_background(SDL_Renderer *r)
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

void draw_starfield(stage *s, SDL_Renderer *r)
{
	int i, c;
	
	for (i = 0 ; i < MAX_STARS ; i++) {
		c = 32 * s->stars[i].speed;
		
		SDL_SetRenderDrawColor(r, c, c, c, 255);
		
		SDL_RenderDrawLine(r, s->stars[i].x, s->stars[i].y, s->stars[i].x + 3, s->stars[i].y);
	}
}

void draw_text(int x, int y, int r, int g, int b, SDL_Renderer *renderer, char *format, ...)
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

void blit(SDL_Texture *texture, int x, int y, SDL_Renderer *r)
{
	SDL_Rect dest;
	dest.x = x;
	dest.y = y;

	SDL_QueryTexture(texture, NULL, NULL, &dest.w, &dest.h);
	SDL_RenderCopy(r, texture, NULL, &dest);
}

void blit_rect(SDL_Texture *texture, SDL_Rect *src, int x, int y, SDL_Renderer *r)
{
	SDL_Rect dest;
	
	dest.x = x;
	dest.y = y;
	dest.w = src->w;
	dest.h = src->h;
	
	SDL_RenderCopy(r, texture, src, &dest);
}
