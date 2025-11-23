#include "gfx_manager.h"
#include "debris.h"
#include "explosion.h"
#include "vec2d.h"
#include "list.h"
#include <stdlib.h>

gfx_manager *init_gfx_manager()
{
	gfx_manager *gm = calloc(1, sizeof(gfx_manager));
	INIT_LIST_HEAD(&gm->debris);
	INIT_LIST_HEAD(&gm->explosions);
	return gm;
}

void gm_add_debris(gfx_manager *gm, entity *e)
{
    int w = e->w / 2;
	int h = e->h / 2;

	for(int y = 0; y <= h; y += h) {
		for(int x = 0; x <= w; x += w) {
			debris * d = create_debris(e, x, y, h, w);
			list_add_tail(&d->list, &gm->debris);
		}
	}
}

void gm_do_debris(gfx_manager *gm)
{
	debris *d, *temp;
	list_for_each_entry_safe(d, temp, &gm->debris, list) {
		vec2d_add(&d->position, &d->velocity);
		if(--d->life <= 0) {
			list_del(&d->list);
			free(d);
		}
	}
}

void gm_add_explosions(gfx_manager *gm, entity *e)
{
	for(int i = 0; i < 20; i++) {
		explosion *ex = create_explosion(e);
		list_add_tail(&ex->list, &gm->explosions);		
	}
}

void gm_do_explosions(gfx_manager *gm)
{
	explosion *ex, *temp;
	list_for_each_entry_safe(ex, temp, &gm->explosions, list) {
		vec2d_add(&ex->position, &ex->velocity);
		if(--ex->a <= 0) {
			list_del(&ex->list);
			free(ex);
		}
	}
}

void gm_clean_gfx(gfx_manager *gm)
{
	explosion *ex, *ex_temp;
	list_for_each_entry_safe(ex, ex_temp, &gm->explosions, list) {
		list_del(&ex->list);
		free(ex);
	}

	debris *d, *d_temp;
	list_for_each_entry_safe(d, d_temp, &gm->explosions, list) {
		list_del(&d->list);
		free(d);
	}
}