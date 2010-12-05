#ifndef _GAME_H
#define _GAME_H
/**
 * Game module
 * 
 * Most of function defined here take an instance of Game as @param
 * @author Gaetan Renaudeau <pro@grenlibre.fr>
 */

#include "globals.h"
#include "points.h"

#define MAX_POSSIBILITIES (4*GRID_SIZE*GRID_SIZE)

/**
 * The whole game structure
 * A game instance store a game state and all infos relative to the game
 */
typedef struct _Game Game;

/**
 * Type of a case : empty or occupied
 */
typedef enum {
  CASE_EMPTY=0,
  CASE_OCCUPIED
} CaseType;

/**
 * Grid infos ready to display (2d grid, cursor position, select case)
 */
typedef struct _Grid {
  CaseType grid[GRID_SIZE][GRID_SIZE];
  Point cursor;
  Point select;
} Grid;

/**
 * A play evalution is an estimation of the quality of a game action
 */
typedef enum {
  PE_NONE=0,
  PE_BAD,
  PE_ORDINARY,
  PE_GREAT,
  PE_IMPRESSIVE,
  PE_AWESOME
} PlayEvaluation;


/**
 * an Action represents a game action event
 * Each action is associate to user interface inputs (keys)
 */
typedef enum {
  Action_NONE=0, /* Action invalid (no action) */
  Action_LEFT, Action_RIGHT, Action_UP, Action_DOWN, /* arrow for navigation */
  Action_YES,
  Action_UNDO,
  Action_TOGGLE_HELP,
  Action_VALID, /* Valid action */
  Action_CANCEL /* Cancel a state (ex: quit the game) */
} Action;

/**
 * Game Mode :
 *  sober: a mode as if you play with paper, pencil and gum
 *  visual: a visual mode with some game helper
 */
typedef enum {
  GM_SOBER=1,
  GM_VISUAL
} GameMode;

// Game general functions

/**
 * Init a game (create a new one)
 */
extern Game* game_init();

/**
 * Close a game
 */
extern void game_close(Game*);

// Game events
/**
 * function called on game start
 */
extern void game_onStart(Game* game);

/**
 * function called on game stop (before closing)
 */
extern void game_onStop(Game* game);

/**
 * function called just before waiting ui action
 */
extern void game_beforeAction(Game* game);

/**
 * function called just after getting a ui action
 * @param action: the action triggered
 * @return true if quit is requested, false else
 */
extern int game_onAction(Game* game, Action action);

// Getters / Setters
/**
 * Get / Set the game mode (sober or visual)
 */
extern GameMode game_getMode(Game* game);
extern void game_setMode(Game* game, GameMode mode);

/**
 * Get the game score
 */
extern int game_getScore(Game* game);

/**
 * Get / Set the select case
 */
extern Point game_getSelect(Game* game);
extern void game_setSelect(Game*, Point);

/**
 * Get / Set the cursor case
 */
extern Point game_getCursor(Game*);
extern void game_setCursor(Game*, Point);

/**
 * Get / Set the player nickname
 */
extern char* game_getNickname(Game* game);
extern void game_setNickname(Game* game, char* nickname);

/**
 * Get / Set the game save file path
 */
extern char* game_getFilepath(Game* game);
extern void game_setFilepath(Game* game, char* filepath);

/**
 * Get the last play evaluation
 */
extern PlayEvaluation game_getLastPlayEvaluation(Game* game);

/**
 * Init a grid
 * @param grid: the grid to init
 */
extern void game_initGrid(Grid* grid);

/**
 * Get the grid of the game
 * @return the grid reference
 */
extern Grid* game_getGrid(Game*);

/**
 * Check if a case is occupied
 * @return true if the case p is occupied
 */
extern int game_isOccupied(Game* game, Point p);

/**
 * Count occupied cases of a line
 * @return the number of occupied cases in the line
 */
extern int game_countOccupiedCases(Game* game, Line);

/**
 * Get the number of possibilities
 * @return the number of line possibilities
 */
extern int game_getPossibilitiesNumber(Game* game);

/**
 * Get all line possibilities
 * @param length: will be setted by the number of possibilities returned
 * @return line possibilities
 */
extern Line* game_getAllPossibilities(Game* game, int* length);

/**
 * Recompute all line possibilities
 * @return line possibilities length
 */
extern int game_computeAllPossibilities(Game* game);

/**
 * Check if the UI must display line possibilities
 * @return true if UI must display line possibilities
 */
extern int game_mustDisplayPossibilities(Game* game);

/**
 * Get all played lines
 * @param length: will be setted by the number of lines returned
 * @return lines of game
 */
extern Line* game_getLines(Game* game, int* length);

/**
 * Get lines number
 * @return number of played lines of game
 */
extern int game_getLinesCount(Game* game);

/**
 * Check if a line is playable
 * @param line: the line to test
 * @return true if line is playable
 */
extern int game_isPlayableLine(Game* game, Line line);

/**
 * Play a line in the game
 * @param line: the line to play
 */
extern void game_consumeLine(Game* game, Line line);

/**
 * Undo last played line
 */
extern void game_undoLine(Game* game);

#endif
