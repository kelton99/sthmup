#ifndef __VECTOR2D__
#define __VECTOR2D__

#include <math.h>

typedef struct vec2d vec2d;

struct vec2d {
	float x;
	float y;
};

vec2d create_vec2d(float x, float y);
void vec2d_add(vec2d *v1, vec2d *v2);
void vec2d_add_points(vec2d *v1, float x, float y);
void vec2d_subtract(vec2d *v1, vec2d *v2);
void vec2d_multiply(vec2d *v1, vec2d *v2);
void vec2d_divide(vec2d *v1, vec2d *v2);
void vec2d_set(vec2d *vec, float x, float y);
void vec2d_scalar(vec2d *v1, float number);
void vec2d_normalize(vec2d *vec);
int vec2d_is_equal(vec2d *v1, vec2d *v2);
float vec2d_length(vec2d *vec);

#endif
