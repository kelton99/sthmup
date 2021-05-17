#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

#include "game.h"
#include "entity.h"
#include "stage.h"
#include "channels.h"
#include "sounds.h"

#define PLAYER_SPEED 4
#define PLAYER_BULLET_SPEED 16
#define CODE event->keysym.scancode

static void doKeyDown(SDL_KeyboardEvent *event, game *g);
static void doKeyUp(SDL_KeyboardEvent *event, game *g);

game *init_game()
{
	
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("Couldn't initialize SDL: %s\n", SDL_GetError());
		exit(1);
	}

	game *g = malloc(sizeof(game));
	
	g->is_running = 1;
	g->window = SDL_CreateWindow("Shooter 01", 0, 0, 1280, 720, 0);
	g->renderer = SDL_CreateRenderer(g->window, -1, SDL_RENDERER_ACCELERATED);
	g->s = init_stage(g->renderer);

	SDL_SetRenderDrawColor(g->renderer, 96, 128, 255, 255);
	
	IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);

	/*
	if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) == -1) {
		printf("Couldn't initialize SDL Mixer\n");
		exit(1);
	}
	Mix_AllocateChannels(MAX_SND_CHANNELS);
	*/

	return g;
}

void handle_input(game *g)
{
	SDL_Event event;
	
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
			case SDL_QUIT:
				g->is_running = 0;
				break;
			case SDL_KEYDOWN:
				doKeyDown(&event.key, g);
				break;
			case SDL_KEYUP:
				doKeyUp(&event.key, g);
			default:
				break;
		}
	}
		
}

void update(game *g)
{
	do_logic(g->keyboard, g->s);
}

void render(game *g)
{
	SDL_RenderClear(g->renderer);
	draw(g->s, g->renderer);
	SDL_RenderPresent(g->renderer);
}

void cleanup(game *g)
{
	SDL_DestroyRenderer(g->renderer);
	
	SDL_DestroyWindow(g->window);
	
	SDL_Quit();
}



static void doKeyDown(SDL_KeyboardEvent *event, game *g)
{
	if (event->repeat == 0 && CODE < MAX_KEYBOARD_KEYS)
		g->keyboard[CODE] = 1;
	
}
static void doKeyUp(SDL_KeyboardEvent *event, game *g)
{
	if (event->repeat == 0 && CODE < MAX_KEYBOARD_KEYS)
		g->keyboard[CODE] = 0;
}