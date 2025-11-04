#ifndef __DEBRIS__
#define __DEBRIS__

#include "entity.h"
#include "vec2d.h"
#include <SDL2/SDL.h>

typedef struct debris debris;

struct debris {
    vec2d position;
    vec2d velocity;
    SDL_Rect rect;
    SDL_Texture *texture;
    int life;
    debris *next;
};

debris *create_debris(entity *e, int x, int y, int h, int w);

#endif