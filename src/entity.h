#ifndef __ENTITY__
#define __ENTITY__

#include <SDL2/SDL.h>
#include "vec2d.h"

typedef struct entity entity;

struct entity {
    vec2d position;
    vec2d velocity;
    int h;
    int w;
	int health;
    int reload;
    int side;
    SDL_Texture *texture;
    entity *next;
};

entity *create_entity(int x, int y, int side, SDL_Texture *texture);

#endif