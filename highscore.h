#ifndef _HIGHSCORE_H
#define _HIGHSCORE_H

#include "globals.h"

typedef struct _Highscore {
  char nickname[NICKNAME_LENGTH+1];
  int score;
} Highscore;

extern int highscore_equals(Highscore *a, Highscore *b);
extern void highscore_sort(Highscore* highscores, int length);
extern int highscore_retrieve(Highscore* highscores, int max);
extern int highscore_store(Highscore* highscores, int length);
extern void highscore_print(Highscore* highscores, int length);

#endif
