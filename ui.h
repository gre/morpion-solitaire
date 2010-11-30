#ifndef _UI_H
#define _UI_H
/**
 * User Interface module
 */
#include "game.h"


/**
 * TODO : some complexe algorithm must be move into others modules (game or other ?)
 * TODO : organize extern functions
 */

/**
 * Blocking function.
 * Wait for a key and return result
 * @return :
 *  the getted ACTION
 */
extern Action ui_getAction();

extern void ui_printInfos(Game* game);

extern void ui_refresh();

extern void ui_printMessage_info(char* str);
extern void ui_printMessage_error(char* str);
extern void ui_printMessage_success(char* str);

extern void ui_init();

extern void ui_close();

extern void ui_updateGrid(Game*);

extern void ui_confirmExit();
extern void ui_cleanMessage();


#endif
