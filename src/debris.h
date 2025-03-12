#ifndef __DEBRIS__
#define __DEBRIS__

#include "entity.h"
#include <SDL2/SDL.h>

typedef struct debris debris;

struct debris {
    float x;
    float y;
    float dx;
    float dy;
    SDL_Rect rect;
    SDL_Texture *texture;
    int life;
    debris *next;
};

debris *create_debris(entity *e);

#endif