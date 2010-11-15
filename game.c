#include <stdlib.h>
#include <math.h>

#include "utils.h"
#include "game.h"


struct _Game {
  Line* lines;
  int nlines;
  
  Grid grid; // computed turn by turn
  int npoints;
  
  Point lastPlay;
  
  int gumModeEnabled;
  GameMode gameMode;
};

static Point noPoint = {-1, -1};


extern Grid* game_getGrid(Game* game) {
  return & (game->grid);
}

extern Game* game_init(GameMode gameMode, int gumModeEnabled) {
  Point p;
  Game* game = malloc(sizeof(Game));
  game->lines = 0;
  game->nlines = 0;
  initGrid(&(game->grid));
  game->npoints = 0;
  for(p.y=0; p.y<GRID_SIZE; ++p.y) {
    for(p.x=0; p.x<GRID_SIZE; ++p.x) {
      if(game->grid.grid[p.x][p.y]!=CASE_EMPTY)
        game->npoints ++;
    }
  }
  game->lastPlay = noPoint;
  game->gumModeEnabled = gumModeEnabled;
  game->gameMode = gameMode;
  return game;
}

extern int game_pointsInSameAxis(Point a, Point b) {
  return a.x==b.x || a.y==b.y;
}
extern int game_pointsInSameDiagonal(Point a, Point b) {
  return util_abs(a.x-b.x) == util_abs(a.y-b.y);
}

extern int game_consumableCases(Game* game, Point from, Point to) {
  int i, dx = to.x-from.x, dy = to.y-from.y, x, y;
  int incrX, incrY;
  if((util_abs(dx)==LINE_LENGTH-1 || util_abs(dx)==LINE_LENGTH-1) 
  && (game_pointsInSameAxis(from, to) || game_pointsInSameDiagonal(from, to))) {
    incrX = dx==0 ? 0 : (dx<0 ? -1 : 1);
    incrY = dy==0 ? 0 : (dy<0 ? -1 : 1);
    ui_printMessage("pass first test");
    for(i=0, x=from.x, y=from.y; i<LINE_LENGTH; x+=incrX, y+=incrY, ++i)
      if(game->grid.grid[x][y]!=CASE_OCCUPIED)
        return FALSE;
    return TRUE;
  }
  else return FALSE;
}
extern void game_consumeCases(Game* game, Point from, Point to) {
  int i, dx = to.x-from.x, dy = to.y-from.y, x, y;
  int incrX = dx==0 ? 0 : (dx<0 ? -1 : 1);
  int incrY = dy==0 ? 0 : (dy<0 ? -1 : 1);
  for(i=0, x=from.x, y=from.y; i<5; x+=incrX, y+=incrY, ++i)
    game->grid.grid[x][y] = CASE_CONSUMED;
}

extern Point game_getCursor(Game* game) {
  return game->grid.cursor;
}
extern void game_setCursor(Game* game, Point p) {
  game->grid.cursor = p;
}
extern int game_isOccupied(Game* game, Point p) {
  return game->grid.grid[p.x][p.y] != CASE_EMPTY;
}
extern void game_occupyCase(Game* game, Point p) {
  game->grid.grid[p.x][p.y] = CASE_OCCUPIED;
}
extern void game_selectCase(Game* game, Point p) {
  game->grid.select = p;
}
extern Point game_getSelect(Game* game) {
  return game->grid.select;
}
extern void game_emptySelection(Game* game) {
  game->grid.select.x = -1;
  game->grid.select.y = -1;
}

extern int pointEquals(Point a, Point b) {
  return a.x==b.x && a.y==b.y;
}
extern int pointExists(Point p) {
  return p.x>=0 && p.y>=0 && p.x<GRID_SIZE && p.y<GRID_SIZE;
}

extern void initGrid(Grid* grid) {
  Point p;
  grid->cursor.x = GRID_SIZE/2;
  grid->cursor.y = GRID_SIZE/2;
  grid->select.x = -1;
  grid->select.y = -1;
  for(p.y=0; p.y<GRID_SIZE; ++p.y) {
    for(p.x=0; p.x<GRID_SIZE; ++p.x) {
      if(((p.y==4||p.y==13) && p.x>=7 && p.x<=10) ||
         ((p.y==7||p.y==10) && ((p.x>=4 && p.x<=7)||(p.x>=10 && p.x<=13))) ||
         ((p.x==4||p.x==13) && p.y>=7 && p.y<=10) ||
         ((p.x==7||p.x==10) && ((p.y>=4 && p.y<=7)||(p.y>=10 && p.y<=13)))) {
        grid->grid[p.x][p.y] = CASE_OCCUPIED;
      }
      else
        grid->grid[p.x][p.y] = CASE_EMPTY;
    }
  }
}
