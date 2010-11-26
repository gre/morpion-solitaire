#ifndef _EXPORT_H
#define _EXPORT_H
/**
 * Save and Load module
 * 
 * (ie = Import/Export)
 */

#include "game.h"

extern int ie_exportGame(Game* game, FILE* file);
extern int ie_importGame(FILE* file, Game* game);

extern int ie_listSavedFiles(char** strings, int limit);
extern FILE* ie_getAvailableFile(char* nickname);

#endif
