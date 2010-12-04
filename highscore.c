#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "globals.h"
#include "highscore.h"

#define HIGHSCORES_PATH "highscores"

static void initHighscoreFile() {
  Highscore hs[10];
  int i;
  for(i=0; i<10; ++i) {
    strcpy(hs[i].nickname, "Computer");
    hs[i].score = (i+1)*10;
  }
  highscore_sort(hs, 10);
  highscore_store(hs, 10);
}

static int comparHighscore(const void* a, const void* b) {
  return ((Highscore*)b)->score - ((Highscore*)a)->score;
}

extern int highscore_equals(Highscore *a, Highscore *b) {
  return a->score==b->score && strcmp(a->nickname, b->nickname)==0;
}

extern void highscore_sort(Highscore* highscores, int length) {
  qsort(highscores, length, sizeof(Highscore), comparHighscore);
}

extern int highscore_retrieve(Highscore* highscores, int max) {
  int length = 0;
  Highscore highscore;
  FILE* file = fopen(HIGHSCORES_PATH, "r");
  if(file==NULL) {
    initHighscoreFile();
    file = fopen(HIGHSCORES_PATH, "r");
  }
  if(file!=NULL) {
    while(length<max && fscanf(file, "%d %" NICKNAME_LENGTH_STR "s", &(highscore.score), highscore.nickname)==2)
      highscores[length++] = highscore;
    fclose(file);
  }
  return length;
}

extern int highscore_store(Highscore* highscores, int length) {
  int i;
  FILE* file = fopen(HIGHSCORES_PATH, "w");
  if(file!=NULL) {
    for(i=0; i<length; ++i)
      fprintf(file, "%d %s\n", highscores[i].score, highscores[i].nickname);
    fclose(file);
  }
  else return 1;
  return 0;
}

extern void highscore_print(Highscore* highscores, int length) {
  int i;
  printf("Place\tScore\tNickname\n");
  for(i=0; i<length; ++i)
    printf(" %d.\t %d\t %s\n", i+1, highscores[i].score, highscores[i].nickname);
}
