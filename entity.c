#include "entity.h"

entity *create_entity(int x, int y, int side, SDL_Texture *texture)
{
    entity *e = calloc(1, sizeof(entity));
    e->x = x;
    e->y = y;
    e->side = side;
    e->health = 1;
    e->texture = texture;
    SDL_QueryTexture(e->texture, NULL, NULL, &e->w, &e->h);
		
    return e;
}
