#ifndef __EXPLOSION__
#define __EXPLOSION__

#include "entity.h"
#include "vec2d.h"

typedef struct explosion explosion;

struct explosion {
    vec2d position;
    vec2d velocity;
    int r, g, b, a;
    list_head list;
};

explosion *create_explosion(entity *e);

#endif