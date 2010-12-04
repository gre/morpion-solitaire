#include <stdio.h>
#include <stdlib.h>

#include "globals.h"
#include "utils.h"
#include "game.h"
#include "ui.h"
#include "export.h"
#include "highscore.h"

typedef enum {
  GES_NONE, GES_ERROR_ONLOAD, GES_FINISHED, GES_INTERRUPT
} GameEndStatus;

static GameEndStatus loadGame(char* filepath);
static GameEndStatus newGame(char* nickname);
static GameEndStatus runGame(Game* game);

static void printHelp(char* argv0) {
  printf("  -----------------------\n");
  printf("  -- Morpion Solitaire --\n");
  printf("  -----------------------\n");
  printf("\n");
  
  printf("Start a new game:\n");
  printf("       %s --new {nickname}\n", argv0);
  printf("       %s  -n {nickname}\n", argv0);
  printf("\n");
  
  printf("Restore a game:\n");
  printf("       %s --load {game file}\n", argv0);
  printf("       %s  -l {game file}\n", argv0);
  printf("\n");

  printf("Show highscores:\n");
  printf("       %s --highscores\n", argv0);
  printf("\n");
  
  printf("Displaying this help:\n");
  printf("       %s --help\n", argv0);
  printf("       %s -h\n", argv0);
  printf("\n");
  
  printf("How to play the game:\n");
  printf(" * Move your cursor \tArrows or ZSQD keys\n");
  printf(" * Do an action \tSpace or Enter key\n");
  printf(" * Quit the game \tEscape then Y key\n");
  printf(" * Toggle the help \tH key\n");
  printf(" * Undo last play \tBackspace key\n");
}

int main(int argc, char* argv[]) {
  GameEndStatus status = GES_NONE;
  char* str = 0;
  Highscore highscores[HIGHSCORE_MAX];
  if(util_containsArg(argc, argv, "--help") || util_containsArg(argc, argv, "-h")) {
    printHelp(argv[0]);
    return 0;
  }
  else if(util_getArgString(argc, argv, "--new", &str)==0 || util_getArgString(argc, argv, "-n", &str)==0) {
    ui_init();
    status = newGame(str);
    ui_close();
  }
  else if(util_getArgString(argc, argv, "--load", &str)==0 || util_getArgString(argc, argv, "-l", &str)==0) {
    ui_init();
    status = loadGame(str);
    ui_close();
  }
  else if(util_containsArg(argc, argv, "--highscores")) {
    highscore_retrieve(highscores, HIGHSCORE_MAX);
    highscore_print(highscores, HIGHSCORE_MAX);
    return 0;
  }
  else {
    printf("\tUsage: %s --new {nickname}\n", argv[0]);
    printf("\tUsage: %s --load {game file}\n", argv[0]);
    printf("More infos with %s --help\n", argv[0]);
    return 0;
  }
  
  if(status==GES_ERROR_ONLOAD) {
    fprintf(stderr, "Unable to load the game.\n");
    return 1;
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
  if(ie_exportGame(game)!=0) { // Be sure we can export the game
    game_close(game);
    return GES_ERROR_ONLOAD;
  }
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
      quitRequest = game_onAction(game, action);
  } while(!end && game_getPossibilitiesNumber(game)>0);
  game_onStop(game);
  return game_getPossibilitiesNumber(game)==0 ? GES_FINISHED : GES_INTERRUPT;
}
