
#ifndef __GAME__
#define __GAME__

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include "entity.h"
#include "highscore.h"
#include "stage.h"
#include "star.h"
#include "defs.h"

typedef struct game {
	SDL_Window *window;
	SDL_Renderer *renderer;
	stage *s;
    star stars[MAX_STARS];
	STATE state;
    highscore_table highscore_table;
	int is_running;
	int keyboard[MAX_KEYBOARD_KEYS];
	char input_text[MAX_LINE_LENGHT];
} game;

game *init_game();
void handle_input(game *g);
void update(game *g);
void render(game *g);
void cleanup(game *g);

#endif
