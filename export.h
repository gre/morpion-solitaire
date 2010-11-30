#ifndef _EXPORT_H
#define _EXPORT_H
/**
 * Save and Load module
 * 
 * (ie = Import/Export)
 */

#include "game.h"

#define FILENAME_BUFFER_SIZE 100


extern int ie_getAvailableFile(char* nickname, char* store);

extern int ie_exportGame(Game* game);
extern int ie_importGame(char* filepath, Game* game);
extern int ie_removeGame(Game* game);

#endif
