#ifndef __DEBRIS__
#define __DEBRIS__

#include "entity.h"

typedef struct debris debris;

struct debris {
    vec2d position;
    vec2d velocity;
    SDL_Rect rect;
    SDL_Texture *texture;
    int life;
    list_head list;
};

debris *create_debris(entity *e, int x, int y, int h, int w);

#endif