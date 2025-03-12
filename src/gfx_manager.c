#include "gfx_manager.h"
#include "defs.h"
#include "entity_manager.h"

gfx_manager *init_gfx_manager()
{
	gfx_manager *gm = calloc(1, sizeof(entity_manager));
	gm->explosion_tail = &gm->explosion_head;
	gm->debris_tail = &gm->debris_head;
	return gm;
}

void gm_add_debris(gfx_manager *gm, entity *e)
{
    int w = e->w / 2;
	int h = e->h / 2;

	for(int y = 0; y <= h; y += h) {
		for(int x = 0; x <= w; x += w) {
			debris *d = calloc(1, sizeof(debris));
			gm->debris_tail->next = d;
			gm->debris_tail = d;

			d->x = e->x + e->w / 2;
			d->y = e->y + e->h / 2;

			if(e->side == SIDE_PLAYER) {
				d->dx = (rand() % 9);
			} else {
				d->dx = -(rand() % 9);
			}
			d->dy = rand() % 3 + (-2) + 1;

			d->life = FPS * 2;
			d->texture = e->texture;

			d->rect.x = x;
			d->rect.y = y;
			d->rect.w = w;
			d->rect.h = h;
		}
	}
}

void gm_do_debris(gfx_manager *gm)
{
    debris *prev = &gm->debris_head;

	for (debris *d = gm->debris_head.next ; d != NULL ; d = d->next) {
		d->x += d->dx;
		d->y += d->dy;

		if (--d->life <= 0) {
			if (d == gm->debris_tail) {
				gm->debris_tail = prev;
			}
			prev->next = d->next;
			free(d);
			d = prev;
		}
		prev = d;
	}
}

void gm_add_explosions(gfx_manager *gm, entity *e)
{
	for(int i = 0; i < 10; i++) {
		explosion *ex = calloc(1, sizeof(explosion));
		gm->explosion_tail->next = ex;
		gm->explosion_tail = ex;

		ex->x = e->x + (rand() % 32) - (rand() % 32);
		ex->y = e->y + (rand() % 32) - (rand() % 32);
		ex->dx = (rand() % 10) - (rand() % 10);
		ex->dy = (rand() % 10) - (rand() % 10);

		// ex->dx /= 10;
		// ex->dy /= 10;

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
	}
}

void gm_do_explosions(gfx_manager *gm)
{
	explosion *prev = &gm->explosion_head;

	for(explosion *e = gm->explosion_head.next; e != NULL; e = e->next) {
		e->x += e->dx;
		e->y += e->dy;
		if(--e->a <= 0) {
			if(e == gm->explosion_tail) {
				gm->explosion_tail = prev;
			}
			prev->next = e->next;
			free(e);
			e = prev;
		}
		prev = e;
	}
}

void gm_clean_gfx(gfx_manager *gm)
{
	while (gm->explosion_head.next) {
		explosion *ex = gm->explosion_head.next;
		gm->explosion_head.next = ex->next;
		free(ex);
	}

	while(gm->debris_head.next) {
		debris *d = gm->debris_head.next;
		gm->debris_head.next = d->next;
		free(d);
	}
}