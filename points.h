#ifndef _POINTS_H
#define _POINTS_H

#include "globals.h"

/**
 * Point and Line module (a line is an array of {LINE_LENGTH} points)
 * 
 * functions are prefixed by point_ or line_
 */

typedef struct _Point {
  int x, y;
} Point;

typedef struct _Line {
  Point points[LINE_LENGTH];
} Line;

extern Point point_empty();
extern Point point_new(int x, int y);
extern int point_equals(Point a, Point b);
// Point exists in the game
extern int point_exists(Point p);
extern int point_indexOf(Point* points, int nb, Point p);

extern int point_inSameAxis(Point a, Point b);
extern int point_inSameDiagonal(Point a, Point b);

extern int line_containsPoint(Line line, Point point);
extern int line_getLineBetween(Point from, Point to, Line* line);
extern int line_isValidLineBetween(Point from, Point to);
extern int line_hasCollinearAndContainsTwo(Line* lines, int nlines, Point a, Point b);
extern int line_hasCollinearAndContains(Line* lines, int nlines, Line line);

#endif
