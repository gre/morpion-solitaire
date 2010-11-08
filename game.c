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


