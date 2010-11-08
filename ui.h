#ifndef _UI_H
#define _UI_H
/**
 * User Interface module
 */

typedef enum {
  NONE=0, /* Action invalid (no action) */
  LEFT, RIGHT, TOP, BOTTOM, /* arrow for navigation */
  VALID, /* Valid action */
  CANCEL /* Cancel a state (ex: quit the game) */
} Action;

/**
 * Blocking function.
 * Wait for a key and return result
 * @return :
 *  the getted ACTION
 */
extern Action ui_getAction();


extern void ui_init();

extern void ui_close();

extern void ui_onGameStart();

extern void ui_onGameResume();

extern void ui_onGameEnd();


extern void ui_updateGrid();



#endif