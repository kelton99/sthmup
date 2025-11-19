#include "vec2d.h"

vec2d create_vec2d(float x, float y)
{
    return (vec2d){.x = x, .y = y};
}

void vec2d_add(vec2d *v1, vec2d *v2)
{
    v1->x += v2->x;
    v1->y += v2->y;
}

void vec2d_add_points(vec2d *vec, float x, float y)
{
    vec->x += x;
    vec->y += y;
}

void vec2d_subtract(vec2d *v1, vec2d *v2)
{
    v1->x -= v2->x;
    v1->y -= v2->y;
}

void vec2d_multiply(vec2d *v1, vec2d *v2)
{
    v1->x *= v2->x;
    v1->y *= v2->y;
}

void vec2d_divide(vec2d *v1, vec2d *v2)
{
    v1->x /= v2->x;
    v1->y /= v2->y;
}

void vec2d_set(vec2d *vec, float x, float y)
{
    vec->x = x;
    vec->y = y;
}

void vec2d_scalar(vec2d *v1, float number)
{
    v1->x *= number;
    v1->y *= number;
}

void vec2d_normalize(vec2d *vec)
{
    float magnitude = vec2d_length(vec);

    if (magnitude > 1) {
		vec->x /= magnitude;
		vec->y /= magnitude;
	}
}

int vec2d_is_equal(vec2d *v1, vec2d *v2)
{
    return v1->x == v2->x && v1->x == v2->y;
}

float vec2d_length(vec2d *vec)
{
    return sqrt(pow(vec->x, 2) + pow(vec->y, 2));
}
