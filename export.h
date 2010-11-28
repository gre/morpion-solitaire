#ifndef _EXPORT_H
#define _EXPORT_H
/**
 * Save and Load module
 * 
 * (ie = Import/Export)
 */

#include "game.h"

#define FILENAME_BUFFER_SIZE 100


extern int ie_exportGame(Game* game);
extern int ie_importGame(char* filepath, Game* game);
extern int ie_removeGame(Game* game);

extern int ie_getAvailableFile(char* nickname, char* store);


// TODO : a highscore module
typedef struct _Highscore {
  char nickname[NICKNAME_LENGTH+1];
  int score;
} Highscore;
extern int ie_storeHighscores(Highscore* highscores, int length);
extern int ie_retrieveHighscores(Highscore* highscores, int max);
extern void ie_sortHighscores(Highscore* highscores, int length);

#endif
