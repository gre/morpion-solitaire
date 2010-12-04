#ifndef _POINTS_H
#define _POINTS_H

#include "globals.h"

/**
 * Point and Line module 
 * functions are prefixed by point_ or line_
 * 
 * @author Gaetan Renaudeau <pro@grenlibre.fr>
 */

/**
 * Point : a 2d coord
 */
typedef struct _Point {
  int x, y;
} Point;

/**
 * a line is an array of {LINE_LENGTH} points
 */
typedef struct _Line {
  Point points[LINE_LENGTH];
} Line;

/**
 * return the empty Point
 */
extern Point point_empty();

/**
 * Create a new point
 * @param x, y: x and y coord of the point
 * @return : a new Point of coord (x, y)
 */
extern Point point_new(int x, int y);

/**
 * Check if two points are equals
 * @param a, b: two point to compar
 * @return true if a equals b
 */
extern int point_equals(Point a, Point b);

/**
 * Check if a point exists in the game
 * @param p: a Point
 * @return true if p is in game bound
 */
extern int point_exists(Point p);

/**
 * Get the index of a point in an array
 * @param points: an array of Point
 * @param nb: the size of the points array
 * @param p: a point to search
 * @return
 *    -1    if p is not in points
 *     i    the first index of p in points : with 0<=i<nb and points[i]=p
 */
extern int point_indexOf(Point* points, int nb, Point p);

/**
 * Check if two points are is the same axis (x or y)
 * @param a, b: two point to compar
 * @return true if points are in the same axis (x or y)
 */
extern int point_inSameAxis(Point a, Point b);

/**
 * Check if two points are is the same diagonal
 * @param a, b: two point to compar
 * @return true if points are in the same diagonal
 */
extern int point_inSameDiagonal(Point a, Point b);

/**
 * Check if a line contains a point
 * @param line
 * @param point
 * @return true if line contains point
 */
extern int line_containsPoint(Line line, Point point);

/**
 * Check if a point is at a line extremity
 * @param line
 * @param point
 * @return true if point is at line extremity
 */
extern int line_pointAtExtremity(Line line, Point point);

/**
 * Make a new line with two points
 * @param from, to: two extremity points
 * @param line: line to make
 * @return the number of points stored into line
 */
extern int line_getLineBetween(Point from, Point to, Line* line);

/**
 * Check if two extremity points are valid to create a new line
 * @param from, to: two extremity points
 * @return true if from and to points are valid to create a line
 */
extern int line_isValidLineBetween(Point from, Point to);

/**
 * Check if a line has collisions (collinear and containing) into lines
 * @param lines: an array of lines
 * @param nlines: length of lines
 * @param line: line to check
 * @return true if line is in collision with lines
 */
extern int line_hasCollinearAndContains(Line* lines, int nlines, Line line);

/**
 * Check if line of two points has collisions (collinear and containing) into lines
 * @param lines: an array of lines
 * @param nlines: length of lines
 * @param a, b: two points forming a line of length = 1
 * @return true if the line (a,b) is in collision with lines
 */
extern int line_hasCollinearAndContainsTwo(Line* lines, int nlines, Point a, Point b);

#endif
