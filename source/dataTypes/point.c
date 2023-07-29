#include "point.h"

#include <stdlib.h>


/* ------------------------------ START PUBLIC ------------------------------ */


/*
@context
    * Initialises a 2D point.

@parameters
    * x
        * X coordinate.
    * y
        * Y coordinate.

@return
    * Point with value.
*/
point_t initPoint(int16_t x,
                  int16_t y)
{
    point_t point;

    point.x = x;
    point.y = y;

    return point;
}


/*
@context
    * Compares 2 points' equality.

@parameters
    * a
        * Point to compare to `b`.
    * b
        * Point to compare to `a`.

@return
    * Indication if `a` and `b` are equal.
*/
bool isEqualPoints(point_t a,
                   point_t b)
{
    return (a.x == b.x) && (a.y == b.y);
}


/*
@context
    * Adds 2 points together element wise.

@parameters
    * a
        * Point to add to `b`.
    * b
        * Point to add to `a`.

@return
    * Point of `a` being added to `b` element wise.
*/
point_t addPoints(point_t a,
                  point_t b)
{
    point_t new;

    new.x = a.x + b.x;
    new.y = a.y + b.y;
    return new;
}


/*
@context
    * Calculates the distance between 2 points in 8 directional movement.
    * Type of distance depends on `costCardinal` and `costDiagonal`.
        * Chebyshev distance when `costCardinal = costDiagonal = 1`.
        * Approximate octile distance when `costCardinal = 70` and
          `costDiagonal = 99`.

@parameters
    * a
        * Point to find distance to `b`.
    * b
        * Point to find distance to `a`.
    * costCardinal
        * Cost of moving in the cardinal direction.
    * costDiagonal
        * Cost of moving in the diagonal direction.

@return
    * Distance between `a` and `b`.
*/
uint16_t distancePoints(point_t  a,
                        point_t  b,
                        uint16_t costCardinal,
                        uint16_t costDiagonal)
{
    uint16_t dx, dy, costDifference;

    dx = abs(a.x - b.x);
    dy = abs(a.y - b.y);
    costDifference = abs(costCardinal - costDiagonal);

    if (dx > dy)
    {
        return (costCardinal * dx) + (costDifference * dy);
    }
    return (costCardinal * dy) + (costDifference * dx);
}


/* ------------------------------- END PUBLIC ------------------------------- */
