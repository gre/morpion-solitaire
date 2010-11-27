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
  GES_ERROR_ONLOAD, GES_FINISHED, GES_INTERRUPT
} GameEndStatus;

static GameEndStatus loadGame(char* filepath);
static GameEndStatus newGame(char* nickname);
static GameEndStatus runGame(Game* game);

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
  
  char* str = 0;
  if(util_getArgString(argc, argv, "--new", &str)==0 || util_getArgString(argc, argv, "-n", &str)==0) {
    ui_init();
    newGame(str);
    ui_close();
  }
  
  if(util_getArgString(argc, argv, "--load", &str)==0 || util_getArgString(argc, argv, "-l", &str)==0) {
    ui_init();
    loadGame(str);
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
  game_setNickname(game, nickname);
  game_setFilepath(game, filepath);
  GameEndStatus status = runGame(game);
  game_close(game);
  return status;
}

// Move it to game and split in many functions.
static GameEndStatus runGame(Game* game) {
  GameEndStatus status;
  int count, countPossibilities = -1;
  Action action = Action_NONE;
  int end = FALSE;
  int cursorChanged;
  int quitRequest = FALSE;
  Point cursor, select;
  Line line;
  int gridNeedUpdate;
  int hasMessage;
  
  ui_onGameStart(game);
   do {
    gridNeedUpdate = FALSE;
    action = ui_getAction();
    ui_cleanMessage();
    hasMessage = FALSE;
    
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
      gridNeedUpdate = TRUE;
    }
    if(action==Action_YES && quitRequest)
      end = TRUE;
    else if(action==Action_VALID) {
      select = game_getSelect(game);
      game_selectCase(game, cursor);
      
      if(game_isValidLineBetween(select, cursor)
      && game_getLineBetween(select, cursor, &line)==LINE_LENGTH) {
        count = game_countOccupiedCases(game, line);
        if((count==LINE_LENGTH || count==LINE_LENGTH-1)
        && game_isPlayableLine(game, line)) {
          ui_printMessage_success("Line played");
          hasMessage = TRUE;
          game_consumeLine(game, line);
          game_emptySelection(game);
          countPossibilities = game_computeAllPossibilities(game);
          ie_exportGame(game);
        }
        else if(pointExists(select)) {
          ui_printMessage_error("Invalid action");
          hasMessage = TRUE;
          game_emptySelection(game);
        }
      }
      else if(pointExists(select)) {
        ui_printMessage_error("Invalid line");
        hasMessage = TRUE;
        game_emptySelection(game);
      }
      
      gridNeedUpdate = TRUE;
    }
    else if(action==Action_UNDO) {
      game_emptySelection(game);
      game_undoLine(game);
      game_computeAllPossibilities(game);
      ie_exportGame(game);
      gridNeedUpdate = TRUE;
    }
    else if(action==Action_TOGGLE_HELP) {
      game_toggleMode(game);
      gridNeedUpdate = TRUE;
    }
    
    if(action==Action_CANCEL) {
      if(pointExists(game_getSelect(game))) {
        game_emptySelection(game);
        gridNeedUpdate = TRUE;
      }
      else {
        quitRequest = TRUE;
        ui_confirmExit();
        hasMessage = TRUE;
      }
    }
    else {
      quitRequest = FALSE;
    }
    
    if(!hasMessage) {
      if(!pointExists(select)) {
        ui_printMessage_info("Select the endpoint of the line by pressing <enter> or <space>");
      }
      else {
        ui_printMessage_info("Select a line startpoint with your cursor by pressing <enter> or <space>");
      }
    }
    
    if(gridNeedUpdate)
      ui_updateGrid(game);
    ui_printInfos(game);
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
