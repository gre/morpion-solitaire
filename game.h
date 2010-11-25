#ifndef _GAME_H
#define _GAME_H
/**
 * Game module
 */

#include "globals.h"

#define MAX_POSSIBILITIES (4*GRID_SIZE*GRID_SIZE)

// TODO : refactoring game (split into multiple files ?)

typedef enum {
  CASE_EMPTY=0,
  CASE_OCCUPIED
} CaseType;

typedef struct _Point {
  int x, y;
} Point;

typedef struct _Grid {
  CaseType grid[GRID_SIZE][GRID_SIZE];
  Point cursor;
  Point select;
} Grid;

typedef struct _Line {
  Point points[LINE_LENGTH];
} Line;

typedef enum {
  GM_SOBER=1,
  GM_VISUAL,
  GM_HELP
} GameMode;


typedef struct _Game Game;

extern Game* game_init();
extern void game_close(Game*);
extern void game_start(Game*);
extern Game* game_startWithLines(Line lines[]);
extern void game_end(Game*);


extern void game_setMode(Game* game, GameMode gameMode);

extern int game_hasCollinearAndContainsTwo(Line* lines, int nlines, Point from, Point to);

extern Line* game_getLines(Game* game, int* length);

extern int game_computeAllPossibilities(Game* game);
extern Line* game_getAllPossibilities(Game* game, int* length);

extern void game_addLine(Game* game, Line l);
extern Grid* game_getGrid(Game*);
extern Point game_getCursor(Game*);
extern int game_isOccupied(Game* game, Point p);
extern int game_countOccupiedCases(Game* game, Point from, Point to);
extern Point game_getSelect(Game* game);
extern void game_occupyCase(Game*, Point);
extern void game_occupyCases(Game*, Point from, Point to);
extern void game_selectCase(Game*, Point);
extern void game_emptySelection(Game* game);
extern void game_setCursor(Game*, Point);
extern void initGrid(Grid*);
extern int game_consumableCases(Game*, Point from, Point to);
extern void game_consumeCases(Game*, Point from, Point to);
extern int pointEquals(Point, Point);
extern int pointExists(Point);

#endif
