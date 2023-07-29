/*
@context
    * Provides a 2D point data type.
*/


#ifndef _POINT_H
    #define _POINT_H

    #include <stdbool.h>
    #include <stdint.h>


    typedef struct point_s point_t;


    struct point_s
    {
        int16_t x, y;
    };


    point_t initPoint(int16_t x,
                      int16_t y);

    bool isEqualPoints(point_t a,
                       point_t b);

    point_t addPoints(point_t a,
                      point_t b);

    uint16_t distancePoints(point_t  a,
                            point_t  b,
                            uint16_t costCardinal,
                            uint16_t costDiagonal);

#endif
