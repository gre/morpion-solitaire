#ifndef _HIGHSCORE_H
#define _HIGHSCORE_H
/**
 * Highscore module
 * @author Gaetan Renaudeau <pro@grenlibre.fr>
 */

#include "globals.h"

/**
 * an Highscore (nickname + score)
 */
typedef struct _Highscore {
  char nickname[NICKNAME_LENGTH+1];
  int score;
} Highscore;

/**
 * Check if two highscores are equals
 * @param a, b: two highscores to compar
 * @return true if a equals b
 */
extern int highscore_equals(Highscore *a, Highscore *b);

/**
 * Sort a highscores array
 * @param highscores: an array of highscore
 * @param length: the length of highscores
 */
extern void highscore_sort(Highscore* highscores, int length);

/**
 * Retrieve highscores from the highscore file
 * @param highscores: array of highscores (to import)
 * @param max: the number limit of highscores to take
 * @return the number of read highscores
 */
extern int highscore_retrieve(Highscore* highscores, int max);

/**
 * Store highscores into the highscore file
 * @param highscores: array of highscores (to store)
 * @param length: highscores length
 * @return 0 if success, 1 else
 */
extern int highscore_store(Highscore* highscores, int length);

/**
 * Print highscores in console mode
 * @param highscores: array of highscores (to store)
 * @param length: highscores length
 */
extern void highscore_print(Highscore* highscores, int length);

#endif
