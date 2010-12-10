#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "game.h"
#include "globals.h"
#include "utils.h"
#include "export.h"
#include "points.h"
#include "highscore.h"

#include "ui.h"

#define LINES_ALLOC_WINDOW 10

static int game_moveCursorForAction(Action action, Point* cursor);
static int game_saveScore(Game* game);
static void game_addLine(Game* game, Line l);

struct _Game {
  Line* lines;
  int nlines;
  int lines_current_max; // lines capacity for current allocation
  
  Grid grid; // computed turn by turn
  
  int score;
  
  Line possibilities[MAX_POSSIBILITIES];
  int possibilities_length;
  
  char* nickname;
  char* filepath;
  
  GameMode mode;
  
  PlayEvaluation lastPlayEvalution;
};

extern Game* game_init() {
  Game* game = malloc(sizeof(Game));
  game->lines = 0;
  game->nlines = 0;
  game->score = 0;
  game->lines_current_max = 0;
  game_initGrid(&(game->grid));
  game->mode = GM_SOBER;
  game->nickname = 0;
  game->filepath = 0;
  game->lastPlayEvalution = PE_NONE;
  game_computeAllPossibilities(game);
  return game;
}

extern void game_close(Game* game) {
  if(game->lines)
    free(game->lines);
  free(game);
}

extern void game_onStart(Game* game) {
  game_computeAllPossibilities(game);
  ui_printMessage_info("Move your cursor with arrows or ZSQD keys");
  ui_updateGrid(game);
  ui_refresh();
}

extern void game_onStop(Game* game) {
  int rank;
  char buf[100], buf2[100];
  if(game_getPossibilitiesNumber(game)==0) {
    rank = game_saveScore(game);
    if(rank)
      snprintf(buf2, 100, " You take the %dth place!", rank);
    else
      *buf2 = 0;
    snprintf(buf, 100, "Game over.%s Press <enter> to quit", buf2);
    ui_printMessage_success(buf);
    ie_removeGame(game);
    ui_updateGrid(game);
    ui_printInfos(game);
    ui_refresh();
    while(ui_getAction() != Action_VALID);
  }
}

extern void game_onActionUndo(Game* game) {
  game_setSelect(game, point_empty());
  game_undoLine(game);
  game->lastPlayEvalution = PE_NONE;
  game_computeAllPossibilities(game);
  ie_exportGame(game);
  ui_printMessage_success("Time machine has done... Going back in time!");
}

extern void game_onActionValid(Game* game) {
  int possibilitiesCount, diff;
  int count;
  Line line;
  Point cursor = game_getCursor(game);
  Point select = game_getSelect(game);
  game_setSelect(game, cursor);
  
  if(line_isValidLineBetween(select, cursor) && line_getLineBetween(select, cursor, &line)==LINE_LENGTH) {
    count = game_countOccupiedCases(game, line);
    if((count==LINE_LENGTH || count==LINE_LENGTH-1) && game_isPlayableLine(game, line)) {
      ui_printMessage_success("Line played. ");
      possibilitiesCount = game_getPossibilitiesNumber(game);
      game_consumeLine(game, line);
      if(game_getLinesCount(game)) {
        diff = game_computeAllPossibilities(game) - possibilitiesCount + 1;
        if(diff<=-2) {
          game->lastPlayEvalution = PE_BAD;
        }
        else if(diff<=1) {
          game->lastPlayEvalution = PE_ORDINARY;
        }
        else if(diff<4) {
          game->lastPlayEvalution = PE_GREAT;
        }
        else if(diff<7) {
          game->lastPlayEvalution = PE_IMPRESSIVE;
        }
        else {
          game->lastPlayEvalution = PE_AWESOME;
        }
        if(count==LINE_LENGTH)
          game->lastPlayEvalution = MIN(PE_AWESOME, game->lastPlayEvalution+3);
      }
      else {
        game->lastPlayEvalution = PE_NONE;
      }
      ie_exportGame(game);
    }
    else if(point_exists(select)) {
      ui_printMessage_error("Invalid action. You better stop now!");
    }
    game_setSelect(game, point_empty());
  }
  else if(point_exists(select)) {
    ui_printMessage_error("Invalid line. You better stop now!");
    game_setSelect(game, point_empty());
  }
}

extern void game_beforeAction(Game* game) {
  ui_updateGrid(game);
  ui_printInfos(game);
  ui_refresh();
}

extern int game_onAction(Game* game, Action action) {
  Point cursor, select;
  if(action==Action_CANCEL && !point_exists(game_getSelect(game))) {
    ui_printMessage_info("Quit? Oh really? [y/n]");
    return TRUE;
  }
  else {
    if(!point_exists(select))
      ui_printMessage_info("Select the endpoint of the line by pressing <enter> or <space>");
    else
      ui_printMessage_info("Select a line startpoint with your cursor by pressing <enter> or <space>");
    
    cursor = game_getCursor(game);
    if(game_moveCursorForAction(action, &cursor))
      game_setCursor(game, cursor);
    
    else if(action==Action_UNDO && game_getLinesCount(game)>0)
      game_onActionUndo(game);
    else if(action==Action_TOGGLE_HELP)
      game_setMode(game, game_getMode(game) == GM_VISUAL ? GM_SOBER : GM_VISUAL); // toggle mode
    else if(action==Action_VALID)
      game_onActionValid(game);
    else if(action==Action_CANCEL)
      game_setSelect(game, point_empty());
    return FALSE;
  }
}

static int game_moveCursorForAction(Action action, Point* cursor) {
  if(action==Action_LEFT && cursor->x>0) {
    cursor->x --;
    return TRUE;
  }
  else if(action==Action_DOWN && cursor->y>0) {
    cursor->y --;
    return TRUE;
  }
  else if(action==Action_RIGHT && cursor->x<GRID_SIZE-1) {
    cursor->x ++;
    return TRUE;
  }
  else if(action==Action_UP && cursor->y<GRID_SIZE-1) {
    cursor->y ++;
    return TRUE;
  }
  return FALSE;
}

extern Grid* game_getGrid(Game* game) {
  return & (game->grid);
}

extern int game_countOccupiedCases(Game* game, Line line) {
  int i, count = 0;
  for(i=0; i<LINE_LENGTH; ++i)
    if(game_isOccupied(game, line.points[i]))
      ++count;
  return count;
}

extern int game_computeAllPossibilities(Game* game) {
  Line* lines = game->possibilities;
  Line line;
  int possibilities = 0;
  int x, y;
  
  for(y=0; y<GRID_SIZE; ++y) {
    for(x=0; x<GRID_SIZE; ++x) {
      if(x>=5) {
        line_getLineBetween(point_new(x-5, y), point_new(x, y), &line);
        if(game_isPlayableLine(game, line))
          lines[possibilities++] = line;
      }
      if(y>=5) {
        line_getLineBetween(point_new(x, y-5), point_new(x, y), &line);
        if(game_isPlayableLine(game, line))
          lines[possibilities++] = line;
      }
      if(x>=5 && y>=5) {
        line_getLineBetween(point_new(x-5, y-5), point_new(x, y), &line);
        if(game_isPlayableLine(game, line))
          lines[possibilities++] = line;
        line_getLineBetween(point_new(x-5, y), point_new(x, y-5), &line);
        if(game_isPlayableLine(game, line))
          lines[possibilities++] = line;
      }
    }
  }
  return game->possibilities_length = possibilities;
}

extern int game_getPossibilitiesNumber(Game* game) {
  return game->possibilities_length;
}

extern Line* game_getAllPossibilities(Game* game, int* length) {
  *length = game->possibilities_length;
  return game->possibilities;
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

extern int game_getScore(Game* game) {
  return game->score;
}

extern void game_setSelect(Game* game, Point p) {
  game->grid.select = p;
}

extern Point game_getSelect(Game* game) {
  return game->grid.select;
}

extern void game_setNickname(Game* game, char* nickname) {
  game->nickname = nickname;
}

extern char* game_getNickname(Game* game) {
  return game->nickname;
}

extern void game_setFilepath(Game* game, char* filepath) {
  game->filepath = filepath;
}

extern char* game_getFilepath(Game* game) {
  return game->filepath;
}

extern int game_getLinesCount(Game* game) {
  return game->nlines;
}

extern GameMode game_getMode(Game* game) {
  return game->mode;
}

extern void game_setMode(Game* game, GameMode mode) {
  game->mode = mode;
}

extern PlayEvaluation game_getLastPlayEvaluation(Game* game) {
  return game->lastPlayEvalution;
}

extern int game_mustDisplayPossibilities(Game* game) {
  return game->mode!=GM_SOBER;
}

extern Line* game_getLines(Game* game, int* length) {
  *length = game->nlines;
  return game->lines;
}

static void game_addLine(Game* game, Line l) {
  if(game->nlines==game->lines_current_max) {
    game->lines_current_max += LINES_ALLOC_WINDOW;
    game->lines = game->lines==0 ? malloc(sizeof(Line)*game->lines_current_max) : realloc(game->lines, sizeof(Line)*game->lines_current_max);
  }
  game->lines[game->nlines++] = l;
}

extern int game_isPlayableLine(Game* game, Line line) {
    int count = game_countOccupiedCases(game, line);
    return ((count==LINE_LENGTH || count==LINE_LENGTH-1) 
    && !line_hasCollinearAndContains(game->lines, game->nlines, line));
}

extern void game_recomputeGrid(Game* game) {
  int i;
  game_initGrid(&(game->grid));
  Line* lines = game->lines;
  int nlines = game->nlines;
  game->score = 0;
  game->lines = 0;
  game->nlines = 0;
  game->lines = malloc(sizeof(Line)*game->lines_current_max);
  for(i=0; i<nlines; ++i) {
    game_consumeLine(game, lines[i]);
  }
  free(lines);
}

extern void game_undoLine(Game* game) {
  Point cursor = game->grid.cursor;
  game->nlines = MAX(game->nlines-1, 0);
  game_recomputeGrid(game);
  game->grid.cursor = cursor;
}

extern void game_consumeLine(Game* game, Line line) {
  int i;
  int count = game_countOccupiedCases(game, line);
  for(i=0; i<LINE_LENGTH; ++i)
    game_occupyCase(game, line.points[i]);
  game->score += (count==LINE_LENGTH) ? POINTS_TRACE_LINE : POINTS_PUT_POINT;
  game_addLine(game, line);
}

static int game_saveScore(Game* game) {
  int rank = 0;
  Highscore highscores[HIGHSCORE_MAX];
  Highscore highscore;
  highscore.score = game_getScore(game);
  strncpy(highscore.nickname, game_getNickname(game), NICKNAME_LENGTH);
  int length = highscore_retrieve(highscores, HIGHSCORE_MAX);
  highscore_sort(highscores, length);
  if(length!=HIGHSCORE_MAX || highscores[length-1].score < highscore.score) {
    highscores[length!=HIGHSCORE_MAX ? length : length-1] = highscore;
    ++ length;
    highscore_sort(highscores, length);
    for(rank=length-1; rank>=0 && !highscore_equals(&highscores[rank], &highscore); --rank);
    rank ++;
  }
  highscore_store(highscores, MIN(HIGHSCORE_MAX, length));
  return rank;
}

extern void game_initGrid(Grid* grid) {
  Point p;
  grid->cursor.x = GRID_SIZE/2;
  grid->cursor.y = GRID_SIZE/2;
  grid->select = point_empty();
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
