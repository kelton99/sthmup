#ifndef __GAME__
#define __GAME__

#include <SDL2/SDL.h>
#include "entity.h"
#include "stage.h"

#define MAX_KEYBOARD_KEYS 350

typedef struct game {
	SDL_Window *window;
	SDL_Renderer *renderer;
	stage *s;
	int is_running;
	int keyboard[MAX_KEYBOARD_KEYS];
} game ;

game *init_game();
void handle_input(game *g);
void update(game *g);
void render(game *g);
void cleanup(game *g);

#endif