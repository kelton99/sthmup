#include "entity.h"
#include "vec2d.h"

entity *create_entity(int x, int y, int side, SDL_Texture *texture)
{
    entity *e = calloc(1, sizeof(entity));
    e->position = create_vec2d(x, y);
    e->velocity = create_vec2d(0, 0);
    e->side = side;
    e->health = 1;
    e->texture = texture;
    SDL_QueryTexture(e->texture, NULL, NULL, &e->w, &e->h);
    return e;
}
