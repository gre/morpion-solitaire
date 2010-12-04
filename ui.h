#ifndef _UI_H
#define _UI_H
/**
 * User Interface module
 * 
 * @author Gaetan Renaudeau <pro@grenlibre.fr>
 */
#include "game.h"

/**
 * Init the User Interface
 */
extern void ui_init();

/**
 * Close the User Interface
 */
extern void ui_close();

/**
 * Refresh the User Interface
 */
extern void ui_refresh();

/**
 * Wait for a key and return result
 * Blocking function.
 * @return : the getted Action ( @see game.h )
 */
extern Action ui_getAction();

/**
 * Print infos of current game state
 * @param game : the current game object
 */
extern void ui_printInfos(Game* game);


/**
 * Print the grid for current game state
 * @param game : the current game object
 */
extern void ui_updateGrid(Game*);

/**
 * _info, _error and _success ui_printMessage functions 
 * display a message with 3 kind of mode (different colors)
 * @param str : the string to display
 */
extern void ui_printMessage_info(char* str);
extern void ui_printMessage_error(char* str);
extern void ui_printMessage_success(char* str);



#endif
