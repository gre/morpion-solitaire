#ifndef _GAME_H
#define _GAME_H
/**
 * Game module
 */

#include "globals.h"

typedef enum {
  EMPTY=0,
  OCCUPIED,
  SELECTED
} CaseType;

typedef struct _Point {
  int x, y;
} Point;

typedef struct _Grid {
  CaseType grid[GRID_SIZE][GRID_SIZE];
  Point hover;
} Grid;

typedef struct _Line {
  Point points[LINE_LENGTH];
} Line;

typedef enum {
  SOBER=1,
  VISUAL,
  HELP
} GameMode;


typedef struct _Game Game;

extern Game* game_init();
extern void game_close(Game*);
extern void game_start(Game*);
extern Game* game_startWithLines(Line lines[]);
extern void game_end(Game*);

#endif
