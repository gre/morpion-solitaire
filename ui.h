#ifndef _UI_H
#define _UI_H
/**
 * User Interface module
 */
#include "game.h"

typedef enum {
  Action_NONE=0, /* Action invalid (no action) */
  Action_LEFT, Action_RIGHT, Action_UP, Action_DOWN, /* arrow for navigation */
  Action_VALID, /* Valid action */
  Action_CANCEL /* Cancel a state (ex: quit the game) */
} Action;

/**
 * Blocking function.
 * Wait for a key and return result
 * @return :
 *  the getted ACTION
 */
extern Action ui_getAction();

extern void ui_refresh();

extern void ui_printMessage(char* str);

extern void ui_init();

extern void ui_close();

extern void ui_onGameStart(Game*);

extern void ui_onGameResume(Game*);

extern void ui_onGameEnd(Game*);

extern void ui_updateGrid(Game*);

extern void ui_confirmExit();
extern void ui_cleanMessage();


#endif