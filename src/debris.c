#include "debris.h"
/*
void do_debris(stage *s) {
	debris *d;
	debris *prev = &s->debris_head;

	for (d = s->debris_head->next ; d != NULL ; d = d->next) {
		d->x += d->dx;
		d->y += d->dy;
		
		if (--d->life <= 0) {
			if (d == s->debris_tail)
				s->debris_tail = prev;
			
			prev->next = d->next;
			free(d);
			d = prev;
		}
		prev = d;
	}
}
*/