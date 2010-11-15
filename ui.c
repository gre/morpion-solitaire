#include <string.h>
#include <stdlib.h>
#include <curses.h>

#include "ui.h"
#include "game.h"

#define ESCAPE_KEY 27
#define ENTER_KEY 10

#define BUFFER_SIZE 64

#define WIN_MESSAGE_WIDTH 50

char buffer[BUFFER_SIZE];

enum { 
  CLR_DEFAULT=1, CLR_CASE, CLR_CASE_CONSUMED, CLR_CASE_HOVER, CLR_CASE_SELECTED
};

WINDOW* win_grid;
WINDOW* win_message;

extern void ui_cleanMessage() {
  mvwhline(win_message, 1, 1, ' ', WIN_MESSAGE_WIDTH-2);
}
extern void ui_printMessage(char* str) {
  ui_cleanMessage();
  mvwprintw(win_message, 1, (WIN_MESSAGE_WIDTH-strlen(str))/2, str);
}

static void setColor(WINDOW* win, int color) {
  wattron(win, COLOR_PAIR(color));
}
static Point toGraphicCoord(Point p) {
  Point newP;
  newP.x = p.x;
  newP.y = GRID_SIZE-p.y-1;
  return newP;
}

static void drawGrid(Grid* grid) {
  Point p;
  Point graphicPoint;
  CaseType caseType;
  for(p.y=0; p.y<GRID_SIZE; ++p.y) {
    for(p.x=0; p.x<GRID_SIZE; ++p.x) {
      caseType = grid->grid[p.x][p.y];
      if(pointEquals(p, grid->select)) {
        setColor(win_grid, CLR_CASE_SELECTED);
      }
      else if(pointEquals(p, grid->cursor)) {
        setColor(win_grid, CLR_CASE_HOVER);
      }
      else {
        if(caseType == CASE_CONSUMED)
          setColor(win_grid, CLR_CASE_CONSUMED);
        else
          setColor(win_grid, CLR_CASE);
      }
      graphicPoint = toGraphicCoord(p);
      mvwprintw(win_grid, graphicPoint.y+1, graphicPoint.x+1, caseType == CASE_EMPTY ? " " : (caseType == CASE_OCCUPIED ? "*" : "o"));
    }
  }
}

extern void ui_refresh() {
  wrefresh(win_grid);
  wrefresh(win_message);
}

extern Action ui_getAction() {
  int ch = wgetch(win_grid);
  switch(ch) {
    case KEY_LEFT:
      return Action_LEFT;
    
    case KEY_RIGHT:
      return Action_RIGHT;
    
    case KEY_UP:
      return Action_UP;
    
    case KEY_DOWN:
      return Action_DOWN;
    
    case 'y':
    case ENTER_KEY:
      return Action_VALID;
    
    case ESCAPE_KEY:
      return Action_CANCEL;
      
    default:
      return Action_NONE;
  }
  return Action_NONE;
}

extern void ui_init(Game* game) {
  initscr();
  if(has_colors() == FALSE)
	{
    endwin();
		fprintf(stderr, "Your terminal does not support color\n");
		exit(1);
	}
  cbreak();
  noecho();
  win_grid = newwin(GRID_SIZE+2, GRID_SIZE+2, 2, 2);
  win_message = newwin(3, WIN_MESSAGE_WIDTH, GRID_SIZE+4, 2);
  keypad(win_grid, TRUE);
  keypad(win_message, TRUE);
  curs_set(0);
  start_color();
  init_pair(CLR_DEFAULT, COLOR_WHITE, COLOR_BLACK);
  init_pair(CLR_CASE, COLOR_BLACK, COLOR_WHITE);
  init_pair(CLR_CASE_CONSUMED, COLOR_GREEN, COLOR_WHITE);
  init_pair(CLR_CASE_HOVER, COLOR_BLACK, COLOR_YELLOW);
  init_pair(CLR_CASE_SELECTED, COLOR_BLACK, COLOR_GREEN);
  box(win_grid, 0, 0);
  box(win_message, 0, 0);
  refresh();
  ui_refresh();
}

extern void ui_confirmExit() {
  ui_printMessage("Quit ? [y/n]");
}

extern void ui_onGameStart(Game* game) {
  ui_updateGrid(game);
}

extern void ui_onGameResume(Game* game) {
  ui_updateGrid(game);
}

extern void ui_onGameEnd(Game* game) { 
}

extern void ui_updateGrid(Game* game) {
  drawGrid(game_getGrid(game));
}

extern void ui_close(Game* game) {
  clrtoeol();
  refresh();
  endwin();
}
