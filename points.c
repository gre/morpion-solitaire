#include "points.h"
#include "utils.h"

static Point noPoint = {-1, -1};

extern Point point_empty() {
  return noPoint;
}
extern Point point_new(int x, int y) {
  Point p;
  p.x = x;
  p.y = y;
  return p;
}
extern int point_equals(Point a, Point b) {
  return a.x==b.x && a.y==b.y;
}
extern int point_exists(Point p) {
  return p.x>=0 && p.y>=0 && p.x<GRID_SIZE && p.y<GRID_SIZE;
}

extern int point_indexOf(Point* points, int nb, Point p) {
  int i;
  for(i=0; i<nb; ++i)
    if(point_equals(points[i], p))
      return i;
  return -1;
}

extern int point_inSameAxis(Point a, Point b) {
  return a.x==b.x || a.y==b.y;
}
extern int point_inSameDiagonal(Point a, Point b) {
  return util_abs(a.x-b.x) == util_abs(a.y-b.y);
}

extern int line_getLineBetween(Point from, Point to, Line* line) {
  int i, dx, dy, incrX, incrY;
  Point p;
  dx = to.x-from.x;
  dy = to.y-from.y;
  incrX = dx==0 ? 0 : (dx<0 ? -1 : 1);
  incrY = dy==0 ? 0 : (dy<0 ? -1 : 1);
  for(i=0, p.x=from.x, p.y=from.y; i<LINE_LENGTH && point_exists(p); p.x+=incrX, p.y+=incrY, ++i)
    line->points[i] = p;
  return i; // count
}

extern int line_isValidLineBetween(Point from, Point to) {
  int dx = to.x-from.x;
  int dy = to.y-from.y;
  return point_exists(from) && point_exists(to)
      && (util_abs(dx)==LINE_LENGTH-1 || util_abs(dy)==LINE_LENGTH-1)
      && (point_inSameAxis(from, to) || point_inSameDiagonal(from, to));
}

extern int line_containsPoint(Line line, Point point) {
  int i;
  for(i=0; i<LINE_LENGTH; ++i)
    if(point_equals(point, line.points[i]))
      return TRUE;
  return FALSE;
}

extern int line_pointAtExtremity(Line line, Point point) {
  return point_equals(line.points[0], point) || point_equals(line.points[LINE_LENGTH-1], point);
}

extern int line_hasCollinearAndContainsTwo(Line* lines, int nlines, Point a, Point b) {
  int i;
  for(i=0; i < nlines; ++i)
    if(line_containsPoint(lines[i], a) && line_containsPoint(lines[i], b))
      return TRUE;
  return FALSE;
}

extern int line_hasCollinearAndContains(Line* lines, int nlines, Line line) {
  Line l;
  int i, j;
  int count;
  
  // foreach lines
  for(i=0; i<nlines; ++i) {
    l = lines[i];
    // if at least two points of l is in line, return true, we found a collision
    count = 0;
    for(j=0; j<LINE_LENGTH; ++j)
      if(line_containsPoint(line, l.points[j]))
        count ++;
    if(count>1)
      return TRUE; 
  }
  return FALSE;
}

