
#include <SDL2/SDL_config_unix.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_scancode.h>
#include <ctype.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include "game.h"
#include "defs.h"
#include "drawer.h"
#include "highscore.h"
#include "sounds.h"
#include "GLOBALS.h"

#define CODE event->keysym.scancode

highscore *new_highscore;
int cursor_blink;

static void do_background();
static void do_starfield(game *g);
static void init_starfield(game *g);
static void doKeyDown(SDL_KeyboardEvent *event, game *g);
static void doKeyUp(SDL_KeyboardEvent *event, game *g);
static void do_name_input(game *g);

game *init_game()
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
		printf("Couldn't initialize SDL: %s\n", SDL_GetError());
		exit(1);
	}

	game *g = malloc(sizeof(game));
	g->s = nullptr;
	g->is_running = 1;
	memset(g->keyboard, 0, sizeof(g->keyboard));
	g->window = SDL_CreateWindow("Shooter 01", 0, 0, 1280, 720, 0);

	if(g->window == NULL) {
		puts("Could not initialize Window");
		exit(EXIT_FAILURE);
	}

	g->renderer = SDL_CreateRenderer(g->window, -1, SDL_RENDERER_ACCELERATED);

	if(g->renderer == NULL) {
		puts("Could not initialize Renderer");
		exit(EXIT_FAILURE);
	}

    if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) == 1) {
        puts("Couldn't initialize SDL Mixer\n");
        exit(EXIT_FAILURE);
    }

	Mix_AllocateChannels(5);
	g->state = HIGHSCORE;
	init_highscore_table(&g->highscore_table);
	init_starfield(g);
	init_draw(g->renderer);
	init_sounds();
	play_music(true);
	IMG_Init(IMG_INIT_PNG);

	cursor_blink = 0;
	return g;
}

void handle_input(game *g)
{
	SDL_Event event;
	memset(g->input_text, '\0', MAX_LINE_LENGHT);

	while (SDL_PollEvent(&event) != 0) {
		switch (event.type) {
			case SDL_QUIT:
				g->is_running = 0;
				break;
			case SDL_KEYDOWN:
				doKeyDown(&event.key, g);
				break;
			case SDL_KEYUP:
				doKeyUp(&event.key, g);
				break;
			case SDL_TEXTINPUT:
				strncpy(g->input_text, event.text.text, MAX_LINE_LENGHT);
				g->input_text[MAX_LINE_LENGHT - 1] = '\0';
				break;
			default:
			    break;
		}
	}
}

void update(game *g)
{
	if(g->keyboard[SDL_SCANCODE_Q]) {
		g->is_running = 0;
	}
	do_background();
	do_starfield(g);

	switch (g->state) {
		case HIGHSCORE:
			if (g->keyboard[SDL_SCANCODE_Z]) {
			    g->s = init_stage();
				g->state = STAGE;
    	    }
			break;
		case STAGE:
			do_stage_logic(g->keyboard, &g->state, &g->highscore_table, &g->s);
			break;
		case INPUT:
            do_name_input(g);
			break;
	}

}

void render(game *g)
{
	SDL_RenderClear(g->renderer);
	draw_background(g->renderer);
	draw_starfield(g->stars, g->renderer);
	switch (g->state) {
		case HIGHSCORE:
			draw_highscore(&g->highscore_table, g->renderer);
			break;
		case STAGE:
			draw_stage(g->s, g->renderer);
			break;
		case INPUT:
		    draw_name_input(g->renderer);
			break;
	}

	SDL_RenderPresent(g->renderer);
}

void cleanup(game *g)
{
	if(g->s) {
		cleanup_stage(g->s);
	}
	SDL_DestroyRenderer(g->renderer);
	SDL_DestroyWindow(g->window);
	free(g);
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

static void init_starfield(game *g)
{
	for(int i = 0; i < MAX_STARS; i++) {
		g->stars[i].x = rand() % SCREEN_WIDTH;
		g->stars[i].y = rand() % SCREEN_HEIGHT;
		g->stars[i].speed = 1 + rand() % 8;
	}
}

static void do_background()
{
	if(--background_x < -SCREEN_WIDTH) {
		background_x = 0;
	}
}

static void do_starfield(game *g)
{
	for(int i = 0; i < MAX_STARS; i++) {
		g->stars[i].x -= g->stars[i].speed;

		if(g->stars[i].x < 0) {
			g->stars[i].x += SCREEN_WIDTH;
		}
	}
}

static void do_name_input(game *g)
{
    int name_lenght = strlen(new_highscore->name);
    printf("%s", g->input_text);
    for (size_t i = 0; i < strlen(g->input_text); ++i) {
        char c = toupper(g->input_text[i]);
        if (name_lenght < MAX_SCORE_NAME_LENGTH - 1
            && name_lenght >= ' ' && c <= 'Z') {
                new_highscore->name[name_lenght++] = c;
        }
    }

    if (name_lenght > 0 && g->keyboard[SDL_SCANCODE_BACKSPACE]) {
        new_highscore->name[--name_lenght] = '\0';
        g->keyboard[SDL_SCANCODE_BACKSPACE] = 0;
    }

    if(g->keyboard[SDL_SCANCODE_RETURN]) {
        if(strlen(new_highscore->name) == 0) {
            strncpy(new_highscore->name, "ANONYMOUS", MAX_SCORE_NAME_LENGTH);
            new_highscore->name[MAX_SCORE_NAME_LENGTH - 1] = '\0';
        }
        new_highscore = nullptr;
        g->state = HIGHSCORE;
    }

    if (++cursor_blink >= FPS) {
        cursor_blink = 0;
    }
}
