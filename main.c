#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

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
static void demo();

static void printHelp(char* argv0) {
  printf("  -----------------------\n");
  printf("  >- Morpion Solitaire -<\n");
  printf("  -----------------------\n");
  printf("\n");
  
  printf("Start a new game:\n");
  printf("       %s --new {nickname}\n", argv0);
  printf("       %s  -n {nickname}\n", argv0);
  printf("\n");
  
  printf("Restore* a game:\n");
  printf("       %s --load {game file}\n", argv0);
  printf("       %s  -l {game file}\n", argv0);
  printf("* note that games are saved into the saved/ directory.\n");
  printf("\n");

  printf("Show highscores:\n");
  printf("       %s --highscores\n", argv0);
  printf("\n");
  
  printf("Start a random game demo:\n");
  printf("       %s --demo\n", argv0);
  printf("       %s -d\n", argv0);
  printf("\n");
  
  printf("Display this help:\n");
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
  }
  else if(util_containsArg(argc, argv, "--demo") || util_containsArg(argc, argv, "-d")) {
    ui_init();
    demo();
    ui_close();
  }
  else {
    printf("\tUsage: %s --new {nickname}\n", argv[0]);
    printf("\tUsage: %s --load {game file}\n", argv[0]);
    printf("\t...\n");
    printf("More infos with %s --help\n", argv[0]);
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

/**
 * Random demo
 */
static void demo() {
  Action action;
  Game* game;
  game = game_init();
  int r;
  int length;
  int end = FALSE;
  Line* lines;
  srand(time(NULL));
  game_setCursor(game, point_empty());
  game_setNickname(game, "this is a random demo");
  ui_printMessage_info("Press any key to continue...");
  do {
    ui_printInfos(game);
    ui_updateGrid(game);
    ui_refresh();
    action = ui_getAction();
    if(action==Action_CANCEL)
      end = TRUE;
    else if(action==Action_TOGGLE_HELP)
      game_setMode(game, game_getMode(game)==GM_SOBER ? GM_VISUAL : GM_SOBER);
    else {
      game_computeAllPossibilities(game);
      lines = game_getAllPossibilities(game, &length);
      if(length>0) {
        r = rand()%length;
        game_consumeLine(game, lines[r]);
      }
    }
  } while(!end && game_getPossibilitiesNumber(game)>0);
  if(!end) {
    ui_printMessage_success("Demo has finished to play. Press <ESC> to quit.");
    ui_refresh();
    while(ui_getAction()!=Action_CANCEL);
  }
  game_close(game);
}

/*
 // Bonus function to bruteforce best score // seems to be 76
static void findBestScore(int try) {
  int best = 0, score;
  while(try-->0) {
    Game* game;
    game = game_init();
    int r;
    int length;
    Line* lines;
    srand(time(NULL));
    game_setCursor(game, point_empty());
    do {
      game_computeAllPossibilities(game);
      lines = game_getAllPossibilities(game, &length);
      if(length>0) {
        r = rand()%length;
        game_consumeLine(game, lines[r]);
      }
    } while(game_getPossibilitiesNumber(game)>0);
    score = game_getScore(game);
    game_close(game);
    if(score>best)
      best = score;
  }
  printf("best score found: %d\n", best);
}
*/
