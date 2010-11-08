#ifndef _EXPORT_H
#define _EXPORT_H
/**
 * Import / Export module
 */

#include "game.h"

extern int ie_exportGame(Game* game, FILE* file);
extern int ie_importGame(FILE* file, Game* game);

#endif
