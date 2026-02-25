
#ifndef __DEFS__
#define __DEFS__

#include <string.h>

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))
#define STRNCPY(dest, src, n) strncpy(dest, src, n); dest[n - 1] = '\0'
#define MAX_KEYBOARD_KEYS 104
#define FPS 60
#define SIDE_PLAYER 0
#define SIDE_ALIEN 1
#define PLAYER_SPEED 4
#define PLAYER_BULLET_SPEED 16
#define ALIEN_BULLET_SPEED 8
#define SCORE_POD_SPEED 5
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define MAX_LINE_LENGHT 2
#define MAX_SCORE_NAME_LENGTH 12

typedef enum STATE {
	HIGHSCORE,
	STAGE,
	INPUT
} STATE;

#endif
