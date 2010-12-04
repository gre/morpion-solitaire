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
  int points[2*LINE_LENGTH];
  for(i=0; i<length; ++i) {
    line = lines[i];
    for(j=0; j<LINE_LENGTH; ++j) {
      points[2*j] = line.points[j].x;
      points[2*j+1] = line.points[j].y;
    }
    fwrite(points, sizeof(int), 2*LINE_LENGTH, file);
  }
  fclose(file);
  return 0;
}
extern int ie_importGame(char* filepath, Game* game) {
  FILE* file = fopen(filepath, "r");
  if(file==NULL) return 1;
  Line line;
  int j, points[2*LINE_LENGTH];
  while(fread(&points, sizeof(int), 2*LINE_LENGTH, file) == 2*LINE_LENGTH) {
    for(j=0; j<LINE_LENGTH; ++j) {
      line.points[j].x = points[2*j];
      line.points[j].y = points[2*j+1];
    }
    game_consumeLine(game, line);
  }
  fclose(file);
  game_setNickname(game, ie_guessNicknameFromFilepath(filepath));
  return 0;
}
extern int ie_removeGame(Game* game) {
  return remove(game_getFilepath(game));
}

