#include <stdio.h>
#include <stdlib.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>

#include <fnmatch.h>
#include <dirent.h>

#include "export.h"
#include "game.h"

#define FILENAME_BUFFER_SIZE 100
#define SAVE_DIR "./saved/"
#define SAVE_FILE_EXTENSION "sav"

extern int ie_exportGame(Game* game, FILE* file) {
  return 0;
}

extern int ie_importGame(FILE* file, Game* game) {
  return 0;
}

extern int ie_listSavedFiles(char** strings, int limit) {
  struct dirent *ent;
  DIR* dir = opendir(SAVE_DIR);
  int count = 0;
  if (dir != NULL) {
		while ((ent=readdir(dir)) != NULL && count < limit) {
			if(ent->d_type!=DT_DIR && fnmatch("*.sav", ent->d_name, 0)==0) {
			  strings[count] = calloc(strlen(ent->d_name)+1, sizeof(char));
        strcpy(strings[count], ent->d_name);
        ++ count;
      }
		}
		closedir (dir);
	}
  return count;
}

/**
 * create a new file for a new game
 */
extern FILE* ie_getAvailableFile(char* nickname) {
  FILE* f;
  int i;
  char buf[FILENAME_BUFFER_SIZE];
  
  struct stat st;
  if(stat(SAVE_DIR, &st) != 0)
    mkdir(SAVE_DIR, 0700); // create save directory if not exists
  
  for(i=1; i<100; ++i) {
    snprintf(buf, FILENAME_BUFFER_SIZE, "%s%s.%02d.%s", SAVE_DIR, nickname, i, SAVE_FILE_EXTENSION);
    f = fopen(buf, "r");
    if(f==NULL) { // file not exists
      fclose(f);
      f = fopen(buf, "w");
      if(f!=NULL)
        return f;
    }
    else
      fclose(f);
  }
  return NULL;
}
