#include "debris.h"
#include "defs.h"

debris *create_debris(entity *e, int x, int y, int h, int w)
{
    debris *d = calloc(1, sizeof(debris));
	d->position = create_vec2d(
	e->position.x + w,
	e->position.y + h
	);

	d->velocity = create_vec2d(rand() % 9, rand() % 3 - 1);

	if(e->side != SIDE_PLAYER) {
		d->velocity.x *= -1;
	}

	d->life = FPS * 2;
	d->texture = e->texture;
	d->rect = (SDL_Rect){ .x = x, .y = y, .w = w, .h = h};
    return d;
}