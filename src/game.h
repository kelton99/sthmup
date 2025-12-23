#ifndef __GAME__
#define __GAME__

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include "entity.h"
#include "stage.h"
#include "star.h"

#define MAX_KEYBOARD_KEYS 350

enum STATE {
	HIGHSCORE,
	STAGE
};

typedef struct game {
	SDL_Window *window;
	SDL_Renderer *renderer;
	stage *s;
    star stars[MAX_STARS];
	enum STATE state;
    highscore_table highscore_table;
	int is_running;
	int keyboard[MAX_KEYBOARD_KEYS];
} game;

game *init_game();
void handle_input(game *g);
void update(game *g);
void render(game *g);
void cleanup(game *g);

#endif
