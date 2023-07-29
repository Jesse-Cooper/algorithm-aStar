/*
@context
    * Provides a method of finding the shortest path between 2 points in a
      dungeon.
    * Uses the A* algorithm with an Octile distance heuristic.
        * The heuristic will never overestimate the actual path cost making it
          admissible.
    * Uses an 8 directional movement system.
    * The path found is dynamically allocated so it must be freed.
*/


#ifndef _A_STAR_H
    #define _A_STAR_H

    #include "dataStructs/dungeon.h"
    #include "dataTypes/point.h"


    point_t *findPath(dungeon_t *dungeon,
                      point_t    source,
                      point_t    target);

#endif
