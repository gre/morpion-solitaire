#include <stdlib.h>
#include <math.h>

#include "utils.h"
#include "game.h"

#define LINES_ALLOC_WINDOW 2

struct _Game {
  Line* lines;
  int nlines;
  int lines_current_max;
  
  Grid grid; // computed turn by turn
  
  Point lastPlay;
  
  int gumModeEnabled;
  GameMode gameMode;
};

static Point noPoint = {-1, -1};

static int game_getLineBetween(Game* game, Point from, Point to, Line* line) {
  int i, dx, dy, incrX, incrY;
  Point p;
  dx = to.x-from.x;
  dy = to.y-from.y;
  incrX = dx==0 ? 0 : (dx<0 ? -1 : 1);
  incrY = dy==0 ? 0 : (dy<0 ? -1 : 1);
  for(i=0, p.x=from.x, p.y=from.y; i<LINE_LENGTH && pointExists(p); p.x+=incrX, p.y+=incrY, ++i)
    line->points[i] = p;
  return i; // count
}

extern void game_addLine(Game* game, Line l) {
  if(game->nlines==game->lines_current_max) {
    game->lines_current_max += LINES_ALLOC_WINDOW;
    game->lines = game->lines==0 ? malloc(sizeof(Line)*game->lines_current_max) : realloc(game->lines, sizeof(Line)*game->lines_current_max);
  }
  game->lines[game->nlines++] = l;
}

extern Line* game_getLines(Game* game, int* length) {
  *length = game->nlines;
  return game->lines;
}

extern Grid* game_getGrid(Game* game) {
  return & (game->grid);
}

extern Game* game_init(GameMode gameMode, int gumModeEnabled) {
  Point p;
  Game* game = malloc(sizeof(Game));
  game->lines = 0;
  game->nlines = 0;
  game->lines_current_max = 0;
  initGrid(&(game->grid));
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
extern int game_countOccupiedCases(Game* game, Point from, Point to) {
  Line line;
  int i, count = 0, size = game_getLineBetween(game, from, to, &line);
  for(i=0; i<size; ++i)
    if(game_isOccupied(game, line.points[i]))
      ++count;
  return count;
}

/*
static int game_pointBetweenExclusif(Point p, Point a, Point b) {
  return util_inRangeExclusif(p.x, a.x, b.x) && util_inRangeExclusif(p.y, a.y, b.y);
}
*/

extern int game_pointsEquals(Point a, Point b) {
  return a.x==b.x && a.y==b.y;
}

extern int game_hasCollinearAndContainsTwo(Game* game, Point from, Point to) {
  Line l;
  int i, j, k;
  int count;
  for(i=0; i < game->nlines; ++i) {
    l = game->lines[i];
    count = 0;
    for(j=0; j<LINE_LENGTH; ++j)
      if(game_pointsEquals(l.points[j], from) || game_pointsEquals(l.points[j], to))
        count ++;
    if(count>1)
      return TRUE;
  }
  return FALSE;
}

extern int game_hasCollinearAndContains(Game* game, Line line) {
  Line l;
  int i, j, k;
  int count;
  
  for(i=0; i < game->nlines; ++i) {
    l = game->lines[i];
    count = 0;
    for(j=0; j<LINE_LENGTH; ++j)
      for(k=0; k<LINE_LENGTH; ++k)
        if(game_pointsEquals(l.points[j], line.points[k]))
          count ++;
    if(count>1)
      return TRUE;
  }
  return FALSE;
}

extern int game_consumableCases(Game* game, Point from, Point to) {
  int dx = to.x-from.x;
  int dy = to.y-from.y;
  Line line;
  // correct axis and distance
  if((util_abs(dx)==LINE_LENGTH-1 || util_abs(dy)==LINE_LENGTH-1) && (game_pointsInSameAxis(from, to) || game_pointsInSameDiagonal(from, to)))
    if(game_getLineBetween(game, from, to, &line)==LINE_LENGTH)
      if(!game_hasCollinearAndContains(game, line))
        return TRUE;
  return FALSE;
}

extern void game_consumeCases(Game* game, Point from, Point to) {
  Line line;
  game_getLineBetween(game, from, to, &line);
  game_addLine(game, line);
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
extern void game_occupyCases(Game* game, Point from, Point to) {
  Line line;
  int i, size = game_getLineBetween(game, from, to, &line);
  for(i=0; i<size; ++i)
    game_occupyCase(game, line.points[i]);
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
