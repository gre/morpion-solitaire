#ifndef _EXPORT_H
#define _EXPORT_H
/**
 * Save and Load module
 * 
 * (functions are prefixed by ie_ for Import/Export)
 * 
 * @author Gaetan Renaudeau <pro@grenlibre.fr>
 */

#include "game.h"

#define FILENAME_BUFFER_SIZE 100

/**
 * Search an available save file path
 * @param nickname: The nickname of the player
 * @param store: the file path
 * @return 0 if success, 1 if no save file was available
 */
extern int ie_getAvailableFile(char* nickname, char* store);

/**
 * Export a game into a file
 * @param game: a game object
 * @return 0 if success, 1 if error
 */
extern int ie_exportGame(Game* game);

/**
 * Import a game from a file
 * @param filepath: the filepath of the saved game
 * @param game: the game instance to import into
 * @return 0 if success, 1 if error
 */
extern int ie_importGame(char* filepath, Game* game);

/**
 * Remove a game saved file
 * @return the result of the remove() call ( @see man remove )
 */
extern int ie_removeGame(Game* game);

#endif
