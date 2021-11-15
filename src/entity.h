#ifndef __ENTITY__
#define __ENTITY__

#include <SDL2/SDL.h>

typedef struct entity entity;

struct entity {
    float x;
    float y;
    float dx;
	float dy;
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