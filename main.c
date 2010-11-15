#include <stdio.h>
#include <stdlib.h>

#include "globals.h"
#include "utils.h"
#include "game.h"
#include "ui.h"
#include "export.h"

int main(int argc, char* argv[]) {
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
    if(action==Action_VALID) {
      if(quitRequest) {
        end = TRUE;
      }
      else {
        if(!game_isOccupied(game, cursor)) {
          game_emptySelection(game);
          game_occupyCase(game, cursor);
        }
        else {
          select = game_getSelect(game);
          if(pointExists(select)) { // game has select
            if(game_consumableCases(game, select, cursor)) {
              ui_printMessage("valid cases");
              game_consumeCases(game, select, cursor);
            }
            else {
              ui_printMessage("invalid selected cases");
            }
          }
          else {
            game_selectCase(game, cursor);
          }
        }
        ui_updateGrid(game);
      }
    }
    
    if(action==Action_CANCEL) {
      quitRequest = TRUE;
      ui_confirmExit();
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
