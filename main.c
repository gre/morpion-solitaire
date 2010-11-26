#include <stdio.h>
#include <stdlib.h>

#include "globals.h"
#include "utils.h"
#include "game.h"
#include "ui.h"
#include "export.h"

/**
 * TODO
 * command line
 *  ./morpion --load [<path to a game file>]
 *  ./morpion --new <username>
 *  ./morpion --help
 *  ./morpion --highscores
 * 
 * menu :
 *  - new game
      * type your name
      * begin a new game and save it in /saved (if exists rename with incremental)
      * start game and save at each turn
 *  - restore game
      * list all games stored in /saved/'* : http://faq.cprogramming.com/cgi-bin/smartfaq.cgi?answer=1046380353&id=1044780608
        display file name & date modification => man 2 stat
 *  - help
 *  - highscores
 *  - quit 
 */

typedef enum {
  GES_FINISHED, GES_INTERRUPT
} GameEndStatus;

static GameEndStatus newGame(GameMode gameMode, char* nickname);
static GameEndStatus runGame(Game* game, FILE* file);

static void printHelp(char* argv0) {
  printf("Usage: %s [--dic <path>] [--level <level>]\n", argv0);
  printf("\n");
  printf("Options:\n");
  printf("  -n [nickname], --new [nickname] \t\tdisplay this help\n");
  printf("  -h, --help\t\tdisplay this help\n");
  printf("\n");
}


int main(int argc, char* argv[]) {
  if(util_containsArg(argc, argv, "--help") 
  || util_containsArg(argc, argv, "-h")) {
    printHelp(argv[0]);
    return 0;
  }
  
  char* str;
  if(util_getArgString(argc, argv, "--new", &str)==0 || util_getArgString(argc, argv, "-n", &str)==0) {
    ui_init();
    newGame(GM_SOBER, str);
    ui_close();
  }
  
  
  return 0;
}

static void homePage() {
  
}

static void highscoresPage() {
  
}

static void startGamePage() {
  
}

static void loadGamePage() {
  
}

static GameEndStatus loadGame(FILE* file) {
  Game* game = game_init();
  ie_importGame(file, game);
  GameEndStatus status = runGame(game, file);
  game_close(game);
  return status;
}

static GameEndStatus newGame(GameMode gameMode, char* nickname) {
  Game* game = game_init();
  game_setMode(game, gameMode);
  str_formatOnlyAlphaAndUnderscore(nickname);
  game_setNickname(game, nickname);
  FILE* f = ie_getAvailableFile(nickname);
  GameEndStatus status = runGame(game, f);
  game_close(game);
  return status;
}

// Move it to game and split in many functions.
static GameEndStatus runGame(Game* game, FILE* file) {
  GameEndStatus status;
  char buf[100];
  int count, countPossibilities = -1;
  int gamePoints = 0;
  Action action = Action_NONE;
  int end = FALSE;
  int cursorChanged;
  int quitRequest = FALSE;
  Point cursor, select;
  
  ui_onGameStart(game);
   do {
    action = ui_getAction();
    ui_cleanMessage();
    
    cursor = game_getCursor(game);
    cursorChanged = FALSE;
    if(action==Action_LEFT && cursor.x>0) {
      cursor.x --;
      cursorChanged = TRUE;
    }
    else if(action==Action_DOWN && cursor.y>0) {
      cursor.y --;
      cursorChanged = TRUE;
    }
    else if(action==Action_RIGHT && cursor.x<GRID_SIZE-1) {
      cursor.x ++;
      cursorChanged = TRUE;
    }
    else if(action==Action_UP && cursor.y<GRID_SIZE-1) {
      cursor.y ++;
      cursorChanged = TRUE;
    }
    
    if(cursorChanged) {
      game_setCursor(game, cursor);
      ui_updateGrid(game);
    }
    if(action==Action_YES && quitRequest)
      end = TRUE;
    else if(action==Action_VALID) {
      select = game_getSelect(game);
      game_selectCase(game, cursor);
      count = game_countOccupiedCases(game, select, cursor);
      if((count==LINE_LENGTH || count==LINE_LENGTH-1)
      && game_consumableCases(game, select, cursor)) {
        gamePoints += (count==LINE_LENGTH) ? POINTS_TRACE_LINE : POINTS_PUT_POINT;
        game_occupyCases(game, select, cursor);
        game_consumeCases(game, select, cursor);
        game_emptySelection(game);
        countPossibilities = game_computeAllPossibilities(game);
        ie_exportGame(game, file);
        sprintf(buf, "Total points: %d", gamePoints);
        ui_printMessage_success(buf);
      }
      else if(pointExists(select)) {
        ui_printMessage_error("invalid action");
        game_emptySelection(game);
      }
      
      ui_updateGrid(game);
    }
    
    if(action==Action_CANCEL) {
      if(pointExists(game_getSelect(game))) {
        game_emptySelection(game);
        ui_updateGrid(game);
      }
      else {
        quitRequest = TRUE;
        ui_confirmExit();
      }
    }
    else {
      quitRequest = FALSE;
    }
    
    ui_refresh();
  } while(!end && countPossibilities!=0);
  
  ui_onGameEnd(game);
  
  if(countPossibilities==0) {
    ui_getAction();
    status = GES_FINISHED;
  }
  else
    status = GES_INTERRUPT;
  return status;
}
