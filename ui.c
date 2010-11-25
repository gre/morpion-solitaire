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

#define GRID_TOP 0

#define MAX(x,y) ((x)>(y) ? (x) : (y))

#define WIN_MESSAGE_WIDTH (4+GRAPHIC_CASE_W*GRID_SIZE)
#define  WIN_MESSAGE_TOP (GRID_TOP+2+(GRAPHIC_CASE_H*GRID_SIZE))

#define GRID_LEFT MAX((COLS-WIN_MESSAGE_WIDTH)/2, 0)

char buffer[BUFFER_SIZE];

enum { 
  CLR_DEFAULT=1, CLR_CASE, CLR_CASE_SELECTED, CLR_CASE_EMPTY_SELECTED, CLR_LINES, CLR_LINES_PLAYABLE,
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

static void drawLines(Line* lines, int nlines) {
  Point a, b, p, p2;
  int hasReverseDiag;
  int x, y, i, j;
  Line line;
  
  for(i=0; i<nlines; ++i) {
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
        // compute a & b to be the reverse diagonal of p & p2
        a = line.points[j];
        b = line.points[j+1];
        if(a.x<b.x){
          a.x++; b.x--;
        }
        else {
          a.x--; b.x++;
        }
        hasReverseDiag = game_hasCollinearAndContainsTwo(lines, nlines, a, b);
        if((p.y<p2.y && p.x<p2.x) || (p.y>p2.y && p.x>p2.x))
          mvwprintw(win_grid, y, x, hasReverseDiag ? "X" : "\\");
        else
          mvwprintw(win_grid, y, x, hasReverseDiag ? "X" :"/");
      }
    }
  }
}

static void cleanGrid() {
  setColor(win_grid, CLR_DEFAULT);
  int i, wLength = GRAPHIC_CASE_W*GRID_SIZE-2, hLength = GRAPHIC_CASE_H*GRID_SIZE-1;
  for(i=1; i<hLength; ++i)
    mvwhline(win_grid, i, 1, ' ', wLength);
}

static void drawGrid(Game* game) {
  Point p, graphicPoint;
  CaseType caseType;
  Grid* grid = game_getGrid(game);
  int length, i, j;
  Line line, *lines;
  int displayPossibilities = TRUE;
  
  cleanGrid();
  
  if(displayPossibilities) {
    lines = game_getAllPossibilities(game, &length);
    setColor(win_grid, CLR_LINES_PLAYABLE);
    drawLines(lines, length);
  }
  
  lines = game_getLines(game, &length);
  setColor(win_grid, CLR_LINES);
  drawLines(lines, length);
  
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
  
  if(displayPossibilities) {
    lines = game_getAllPossibilities(game, &length);
    for(i=0; i<length; ++i) {
      line = lines[i];
      for(j=0; j<LINE_LENGTH; ++j) {
        p = line.points[j];
        graphicPoint = toGraphicCoord(p);
        caseType = grid->grid[p.x][p.y];
        if(caseType == CASE_EMPTY) {
          if(pointEquals(p, grid->cursor)) 
            wattron(win_grid, A_REVERSE);
          else 
            wattroff(win_grid, A_REVERSE);
          setColor(win_grid, pointEquals(p, grid->select) ? CLR_CASE_SELECTED : CLR_LINES_PLAYABLE);
          mvwprintw(win_grid, graphicPoint.y+1, graphicPoint.x+2, "*");
        }
      }
    }
  }
  
  wattroff(win_grid, A_REVERSE);
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
    case 'q':
      return Action_LEFT;
    
    case KEY_RIGHT:
    case 'd':
      return Action_RIGHT;
    
    case KEY_UP:
    case 'z':
      return Action_UP;
    
    case KEY_DOWN:
    case 's':
      return Action_DOWN;
    
    case ENTER_KEY:
    case ' ':
      return Action_VALID;
    
    case 'y':
      return Action_YES;
    
    case ESCAPE_KEY:
      return Action_CANCEL;
    
    default:
      ui_refresh();
      return Action_NONE;
  }
  return Action_NONE;
}

extern void ui_init() {
  initscr();
  ESCDELAY = 0;
  if(has_colors() == FALSE) {
    endwin();
		fprintf(stderr, "Your terminal does not support color\n");
		exit(1);
	}
  cbreak();
  noecho();
  win_grid = newwin(GRAPHIC_CASE_H*(GRID_SIZE+1), GRAPHIC_CASE_W*(GRID_SIZE+1), GRID_TOP, GRID_LEFT);
  win_message = newwin(3, WIN_MESSAGE_WIDTH, WIN_MESSAGE_TOP, GRID_LEFT);
  keypad(win_grid, TRUE);
  keypad(win_message, TRUE);
  curs_set(0);
  start_color();
  init_pair(CLR_DEFAULT, COLOR_WHITE, COLOR_BLACK);
  init_pair(CLR_LINES, COLOR_YELLOW, COLOR_BLACK);
  init_pair(CLR_LINES_PLAYABLE, COLOR_BLUE, COLOR_BLACK);
  init_pair(CLR_CASE, COLOR_WHITE, COLOR_BLACK);
  init_pair(CLR_CASE_SELECTED, COLOR_GREEN, COLOR_BLACK);
  init_pair(CLR_CASE_EMPTY_SELECTED, COLOR_BLUE, COLOR_GREEN);
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
