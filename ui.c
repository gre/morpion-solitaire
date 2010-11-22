#include <string.h>
#include <stdlib.h>
#include <curses.h>

#include "ui.h"
#include "game.h"

#define ESCAPE_KEY 27
#define ENTER_KEY 10

#define BUFFER_SIZE 64

#define GRAPHIC_CASE_H 2
#define GRAPHIC_CASE_W 4

#define WIN_MESSAGE_WIDTH (4+GRAPHIC_CASE_W*GRID_SIZE)
#define  WIN_MESSAGE_TOP (4+(GRAPHIC_CASE_H*GRID_SIZE))

char buffer[BUFFER_SIZE];

enum { 
  CLR_DEFAULT=1, CLR_CASE, CLR_CASE_SELECTED, CLR_CASE_EMPTY_SELECTED, CLR_LINES,
  CLR_MESSAGE, CLR_MESSAGE_ERROR, CLR_MESSAGE_SUCCESS
};

WINDOW* win_grid;
WINDOW* win_message;


extern void setColor(WINDOW* win, int color) {
  wattron(win, COLOR_PAIR(color));
}

extern void ui_cleanMessage() {
  mvwhline(win_message, 1, 1, ' ', WIN_MESSAGE_WIDTH-2);
}
static void ui_printMessage(char* str) {
  ui_cleanMessage();
  mvwprintw(win_message, 1, (WIN_MESSAGE_WIDTH-strlen(str))/2, str);
}

extern void ui_printMessage_info(char* str) {
  setColor(win_message, CLR_MESSAGE);
  ui_printMessage(str);
}
extern void ui_printMessage_success(char* str) {
  setColor(win_message, CLR_MESSAGE_SUCCESS);
  ui_printMessage(str);
}
extern void ui_printMessage_error(char* str) {
  setColor(win_message, CLR_MESSAGE_ERROR);
  ui_printMessage(str);
}

static Point toGraphicCoord(Point p) {
  Point newP;
  newP.x = 2+GRAPHIC_CASE_W*p.x;
  newP.y = 1+GRAPHIC_CASE_H*(GRID_SIZE-p.y-1);
  return newP;
}

static void drawGrid(Game* game) {
  int currentChar;
  int i, j, x, y;
  int length;
  Point p, p2;
  Point graphicPoint;
  CaseType caseType;
  Grid* grid = game_getGrid(game);
  Line* lines = game_getLines(game, &length);
  Line line;
  
  for(p.y=0; p.y<GRID_SIZE; ++p.y) {
    for(p.x=0; p.x<GRID_SIZE; ++p.x) {
      caseType = grid->grid[p.x][p.y];
      if(pointEquals(p, grid->cursor)) 
        wattron(win_grid, A_REVERSE);
      else 
        wattroff(win_grid, A_REVERSE);
      setColor(win_grid, (pointEquals(p, grid->select)) ? (caseType == CASE_EMPTY ? CLR_CASE_EMPTY_SELECTED : CLR_CASE_SELECTED) : CLR_CASE);
    
      graphicPoint = toGraphicCoord(p);
      mvwprintw(win_grid, graphicPoint.y+1, graphicPoint.x+2, caseType == CASE_EMPTY ? " " : "o");
    }
  }
  
  setColor(win_grid, CLR_LINES);
  for(i=0; i<length; ++i) {
    line = lines[i];
    for(j=0; j<LINE_LENGTH-1; ++j) {
      p = toGraphicCoord(line.points[j]);
      p2 = toGraphicCoord(line.points[j+1]);
      x = (p.x+p2.x)/2 + 2;
      y = (p.y+p2.y)/2 + 1;
      if(p.x==p2.x)
        mvwprintw(win_grid, y, x, ":");
      else if(p.y==p2.y)
        mvwprintw(win_grid, y, x-1, "---");
      else {
        // game_hasCollinearAndContainsTwo(game, line.points[j], line.points[j+1])
        if((p.y<p2.y && p.x<p2.x) || (p.y>p2.y && p.x>p2.x))
          mvwprintw(win_grid, y, x, currentChar=='/'||currentChar=='X' ? "X" : "\\");
        else
          mvwprintw(win_grid, y, x-1, currentChar=='\\'||currentChar=='X' ? "X" :"/");
      }
    }
  }
}

extern void ui_refresh() {
  wrefresh(win_grid);
  wrefresh(win_message);
  refresh();
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
      ui_refresh();
      return Action_NONE;
  }
  return Action_NONE;
}

extern void ui_init(Game* game) {
  initscr();
  ESCDELAY = 0;
  if(has_colors() == FALSE) {
    endwin();
		fprintf(stderr, "Your terminal does not support color\n");
		exit(1);
	}
  cbreak();
  noecho();
  win_grid = newwin(GRAPHIC_CASE_H*(GRID_SIZE+1), GRAPHIC_CASE_W*(GRID_SIZE+1), 2, 2);
  win_message = newwin(3, WIN_MESSAGE_WIDTH, WIN_MESSAGE_TOP, 2);
  keypad(win_grid, TRUE);
  keypad(win_message, TRUE);
  curs_set(0);
  start_color();
  init_pair(CLR_DEFAULT, COLOR_WHITE, COLOR_BLACK);
  init_pair(CLR_LINES, COLOR_BLUE, COLOR_BLACK);
  init_pair(CLR_CASE, COLOR_WHITE, COLOR_BLACK);
  init_pair(CLR_CASE_SELECTED, COLOR_GREEN, COLOR_BLACK);
  init_pair(CLR_CASE_EMPTY_SELECTED, COLOR_YELLOW, COLOR_GREEN);
  init_pair(CLR_MESSAGE, COLOR_WHITE, COLOR_BLACK);
  init_pair(CLR_MESSAGE_ERROR, COLOR_RED, COLOR_BLACK);
  init_pair(CLR_MESSAGE_SUCCESS, COLOR_GREEN, COLOR_BLACK);
  
  box(win_grid, 0, 0);
  box(win_message, 0, 0);
  refresh();
  ui_refresh();
}

extern void ui_confirmExit() {
  ui_printMessage_info("Quit ? [y/n]");
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
  drawGrid(game);
}

extern void ui_close(Game* game) {
  clrtoeol();
  refresh();
  delwin(win_grid);
  delwin(win_message);
  endwin();
}
