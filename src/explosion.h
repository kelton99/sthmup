#ifndef __EXPLOSION__
#define __EXPLOSION__

typedef struct explosion explosion;

struct explosion {
    float x;
    float y;
    float dx;
    float dy;
    int r, g, b, a;
    explosion *next;
};

#endif