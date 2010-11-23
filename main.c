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
 *  ./morpion --new
 *  ./morpion --help
 *  ./morpion --highscores
 * 
 * menu :
 *  - new game
      * type your name
      * begin a new game and save it in /saved (if exists rename with incremental)
      * start game and save at each turn
 *  - restore game
      * list all games stored in /saved/* : http://faq.cprogramming.com/cgi-bin/smartfaq.cgi?answer=1046380353&id=1044780608
        display file name & date modification => man 2 stat
 *  - help
 *  - highscores
 *  - quit 
 */

int main(int argc, char* argv[]) {
  int count;
  int gamePoints = 0;
  char buf[100];
  
  Action action = Action_NONE;
  int end = FALSE;
  int cursorChanged;
  int quitRequest = FALSE;
  Point cursor, select;
  Game* game = game_init(GM_SOBER, TRUE);
  ui_init(game);
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
  } while(!end);
  
  ui_onGameEnd(game);
  ui_close();
  return 0;
}
