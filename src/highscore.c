#include "highscore.h"
#include "defs.h"
#include "GLOBALS.h"
#include <stdlib.h>
#include <string.h>

static int highscore_comparator(const void *a, const void *b)
{
    highscore *h1 = ((highscore*)a);
    highscore *h2 = ((highscore*)b);

    return h2->score - h1->score;
}

void init_highscore_table(highscore_table *t)
{
	memset(t, 0, sizeof(highscore_table));
	for(int i = 0; i < NUM_HIGHSCORES; i++) {
		t->highscore[i].score = 0;
        STRNCPY(t->highscore[i].name, "..........", NUM_HIGHSCORES);
	}
	new_highscore = nullptr;
    //cursor_blink = 0;
}

void add_highscore(highscore_table *table, int score)
{
    highscore new_highscores[NUM_HIGHSCORES + 1];

    memset(new_highscores, 0, sizeof(highscore) * (NUM_HIGHSCORES + 1));

    for (int i = 0; i < NUM_HIGHSCORES; i++) {
        new_highscores[i] = table->highscore[i];
        new_highscores[i].recent = 0;
    }

    new_highscores[NUM_HIGHSCORES].score = score;
    new_highscores[NUM_HIGHSCORES].recent = 1;

    qsort(new_highscores, NUM_HIGHSCORES + 1, sizeof(highscore), highscore_comparator);
    new_highscore = nullptr;
    for(int i = 0; i < NUM_HIGHSCORES; i++) {
        table->highscore[i] = new_highscores[i];
        if (table->highscore[i].recent) {
            new_highscore = &table->highscore[i];
        }
    }
}
