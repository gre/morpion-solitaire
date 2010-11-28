#include <stdio.h>
#include <stdlib.h>

#include "globals.h"
#include "utils.h"
#include "game.h"
#include "ui.h"
#include "export.h"

/**
 * TODO
 *
 * impl help mode : on case play hover display playable other case
 */

typedef enum {
  GES_ERROR_ONLOAD, GES_FINISHED, GES_INTERRUPT
} GameEndStatus;

static GameEndStatus loadGame(char* filepath);
static GameEndStatus newGame(char* nickname);
static GameEndStatus runGame(Game* game);

static void printHelp(char* argv0) {
  printf("  -----------------------\n");
  printf("  -- Morpion Solitaire --  2010 -- Gaetan Renaudeau\n");
  printf("  -----------------------\n");
  printf("\n");
  
  printf("Start a new game :\n");
  printf("       %s --new {nickname}\n", argv0);
  printf("       %s  -n {nickname}\n", argv0);
  printf("\n");
  
  printf("Restore a game :\n");
  printf("       %s --load {game file}\n", argv0);
  printf("       %s  -l {game file}\n", argv0);
  printf("\n");

  printf("Options:\n");
  printf("  --highscores\t\tshow highscores\n");
  printf("  --help, -h\t\tdisplay this help\n");
  printf("\n");
}

int main(int argc, char* argv[]) {
  char* str = 0;
  if(util_containsArg(argc, argv, "--help") || util_containsArg(argc, argv, "-h")) {
    printHelp(argv[0]);
    return 0;
  }
  else if(util_getArgString(argc, argv, "--new", &str)==0 || util_getArgString(argc, argv, "-n", &str)==0) {
    ui_init();
    newGame(str);
    ui_close();
  }
  else if(util_getArgString(argc, argv, "--load", &str)==0 || util_getArgString(argc, argv, "-l", &str)==0) {
    ui_init();
    loadGame(str);
    ui_close();
  }
  else if(util_getArgString(argc, argv, "--highscores", &str)==0) {
    // TODO
    printf("TODO: print highscoress\n");
  }
  else {
    printf("\tUsage: %s --new {nickname}\n", argv[0]);
    printf("\tUsage: %s --load {game file}\n", argv[0]);
    printf("More infos with %s --help\n", argv[0]);
  }
  return 0;
}

static GameEndStatus loadGame(char* filepath) {
  Game* game = game_init();
  if(ie_importGame(filepath, game)!=0) {
    game_close(game);
    return GES_ERROR_ONLOAD;
  }
  game_setFilepath(game, filepath);
  GameEndStatus status = runGame(game);
  game_close(game);
  return status;
}

static GameEndStatus newGame(char* nickname) {
  Game* game;
  char filepath[FILENAME_BUFFER_SIZE];
  if(ie_getAvailableFile(nickname, filepath)!=0)
    return GES_ERROR_ONLOAD;
  game = game_init();
  str_formatOnlyAlphaAndUnderscore(nickname);
  str_truncate(nickname, NICKNAME_LENGTH);
  game_setNickname(game, nickname);
  game_setFilepath(game, filepath);
  GameEndStatus status = runGame(game);
  game_close(game);
  return status;
}

// Move it to game and split in many functions.
static GameEndStatus runGame(Game* game) {
  Action action;
  int end = FALSE, quitRequest = FALSE;
  game_onStart(game);
  do {
    game_beforeAction(game);
    action = ui_getAction();
    if(action==Action_YES && quitRequest)
      end = TRUE;
    else
      game_onAction(game, action, &quitRequest);
  } while(!end && game_getPossibilitiesNumber(game)>0);
  game_onStop(game);
  return game_getPossibilitiesNumber(game)==0 ? GES_FINISHED : GES_INTERRUPT;
}
