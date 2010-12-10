#include <stdio.h>
#include <stdlib.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>

#include "export.h"
#include "game.h"
#include "globals.h"

#define SAVE_DIR "saved/"
#define SAVE_FILE_EXTENSION "sav"


static char* ie_guessNicknameFromFilepath(char* filepath) {
  int start, end, len;
  len = strlen(filepath);
  for(start = len-1; start>=0 && filepath[start]!='/'; --start);
  ++ start;
  for(end = start; end<len && filepath[end]!='.'; ++end);
  char* nickname = malloc(sizeof(char)*(end-start+1));
  strncpy(nickname, filepath+start, end-start);
  nickname[end-start] = 0;
  return nickname;
}

extern int ie_getAvailableFile(char* nickname, char* store) {
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
      strcpy(store, buf);
      return 0;
    }
    else
      fclose(f);
  }
  return 1;
}

extern int ie_exportGame(Game* game) {
  FILE* file = fopen(game_getFilepath(game), "w");
  if(file==NULL) return 1;
  int length, i, j;
  Line line, *lines = game_getLines(game, &length);
  for(i=0; i<length; ++i) {
    line = lines[i];
    for(j=0; j<LINE_LENGTH; ++j) {
      fprintf(file, "%d %d", line.points[j].x, line.points[j].y);
      if(j<LINE_LENGTH-1)
        fprintf(file, " ");
    }
    fprintf(file, "\n");
  }
  fclose(file);
  return 0;
}
extern int ie_importGame(char* filepath, Game* game) {
  FILE* file = fopen(filepath, "r");
  if(file==NULL) return 1;
  Line line;
  int i, j, length, count;
  Point p;
  do {
    length = 0;
    for(i=0; i<LINE_LENGTH && (count=fscanf(file, "%d %d", &p.x, &p.y)); ++i) {
      length += count;
      line.points[i] = p;
    }
    if(length==2*LINE_LENGTH) {
      for(j=0; j<LINE_LENGTH; ++j)
        if(!point_exists(line.points[j])) {
          fclose(file);
          return 2;
        }
      game_consumeLine(game, line);
    }
  } while(length==2*LINE_LENGTH);
  fclose(file);
  game_setNickname(game, ie_guessNicknameFromFilepath(filepath));
  return 0;
}
extern int ie_removeGame(Game* game) {
  return remove(game_getFilepath(game));
}

