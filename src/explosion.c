#include "explosion.h"
#include "defs.h"

explosion *create_explosion(entity *e)
{
    explosion *ex = calloc(1, sizeof(explosion));

	ex->position = create_vec2d(
	    e->position.x + (rand() % 32) - (rand() % 32),
	    e->position.y + (rand() % 32) - (rand() % 32)
	);
	ex->velocity = create_vec2d(
		(rand() % 10) - (rand() % 10), 
		(rand() % 10) - (rand() % 10)
	);

    switch (rand() % 4) {
		case 0:
			ex->r = 255;
			break;
		case 1:
			ex->r = 255;
			ex->g = 128;
			break;
		case 2:
			ex->r = 255;
			ex->g = 255;
			break;
		default:
			ex->r = 255;
			ex->g = 255;
			ex->b = 255;
			break;
	}
	ex->a = rand() % FPS * 3;

    return ex;
}