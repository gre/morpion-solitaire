#ifndef _GAME_H
#define _GAME_H
/**
 * Game module
 */

#include "globals.h"
#include "points.h"

#define MAX_POSSIBILITIES (4*GRID_SIZE*GRID_SIZE)

typedef enum {
  CASE_EMPTY=0,
  CASE_OCCUPIED
} CaseType;

typedef struct _Grid {
  CaseType grid[GRID_SIZE][GRID_SIZE];
  Point cursor;
  Point select;
} Grid;

typedef enum {
  Action_NONE=0, /* Action invalid (no action) */
  Action_LEFT, Action_RIGHT, Action_UP, Action_DOWN, /* arrow for navigation */
  Action_YES,
  Action_UNDO,
  Action_TOGGLE_HELP,
  Action_VALID, /* Valid action */
  Action_CANCEL /* Cancel a state (ex: quit the game) */
} Action;

typedef enum {
  GM_SOBER=1,
  GM_VISUAL,
  GM_HELP
} GameMode;

typedef struct _Game Game;


// Game general functions
extern Game* game_init();
extern void game_close(Game*);

// Game event
extern void game_onStop(Game* game);
extern void game_onStart(Game* game);
extern void game_end(Game*);
extern void game_beforeAction(Game* game);
extern void game_onAction(Game* game, Action action, int* quitRequest);

// Getters / Setters
extern GameMode game_getMode(Game* game);
extern void game_setMode(Game* game, GameMode mode);

extern int game_getScore(Game* game);
extern int game_saveScore(Game* game);

extern Point game_getSelect(Game* game);
extern void game_setSelect(Game*, Point);

extern Point game_getCursor(Game*);
extern void game_setCursor(Game*, Point);

extern char* game_getNickname(Game* game);
extern void game_setNickname(Game* game, char* nickname);

extern char* game_getFilepath(Game* game);
extern void game_setFilepath(Game* game, char* filepath);

extern GameMode game_toggleMode(Game* game);

extern void game_initGrid(Grid*);
extern Grid* game_getGrid(Game*);
extern int game_isOccupied(Game* game, Point p);
extern int game_countOccupiedCases(Game* game, Line);

extern int game_getPossibilitiesNumber(Game* game);
extern Line* game_getAllPossibilities(Game* game, int* length);
extern int game_mustDisplayPossibilities(Game* game);

extern Line* game_getLines(Game* game, int* length);
extern int game_getLinesCount(Game* game);
extern void game_addLine(Game* game, Line l);

extern int game_isPlayableLine(Game* game, Line line);
extern void game_consumeLine(Game*, Line);
extern void game_undoLine(Game* game);

#endif
