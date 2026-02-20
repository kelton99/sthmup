#ifndef __HIGHSCORE__
#define __HIGHSCORE__

#include "defs.h"
#include <string.h>

#define NUM_HIGHSCORES 8

typedef struct {
    char name[MAX_LINE_LENGHT];
    int recent;
    int score;
} highscore;

typedef struct {
    highscore highscore[NUM_HIGHSCORES];
} highscore_table;

void init_highscore_table(highscore_table *t);
void add_highscore(highscore_table *table, int score);

#endif