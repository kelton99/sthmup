#include "gfx_manager.h"
#include "debris.h"
#include "entity_manager.h"
#include "explosion.h"
#include "vec2d.h"

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
			debris * d = create_debris(e, x, y, h, w);
			gm->debris_tail->next = d;
			gm->debris_tail = d;
		}
	}
}

void gm_do_debris(gfx_manager *gm)
{
    debris *prev = &gm->debris_head;

	for (debris *d = gm->debris_head.next ; d != NULL ; d = d->next) {
		vec2d_add(&d->position, &d->velocity);

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
	for(int i = 0; i < 20; i++) {
		explosion *ex = create_explosion(e);
		gm->explosion_tail->next = ex;
		gm->explosion_tail = ex;
		
	}
}

void gm_do_explosions(gfx_manager *gm)
{
	explosion *prev = &gm->explosion_head;

	for(explosion *e = gm->explosion_head.next; e != NULL; e = e->next) {
		vec2d_add(&e->position, &e->velocity);
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