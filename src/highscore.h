#ifndef __HIGHSCORE__
#define __HIGHSCORE__

#include <string.h>
#define NUM_HIGHSCORES 8

typedef struct {
    int recent;
    int score;
} highscore;

typedef struct {
    highscore highscore[NUM_HIGHSCORES];
} highscore_table;

void do_highscore();

#endif