/*
@context
    * Provides a dungeon data structure.
    * Can generate random dungeon configurations.
        * Generated drawing lines between random sequences of points.
        * Source and target separated by `SOURCE_TARGET_SEP`.
        * Either width or height must be `>= MIN_SIZE`.
    * As a dungeon is random, setting seed in `stdlib` may change the result.
*/


#ifndef _DUNGEON_H
    #define _DUNGEON_H

    #include <stdbool.h>
    #include <stdint.h>

    #include "../dataTypes/point.h"


    typedef struct dungeon_s dungeon_t;


    dungeon_t *initDungeon(uint16_t width,
                           uint16_t height);

    void freeDungeon(dungeon_t *dungeon);

    uint16_t getDungeonWidth(dungeon_t *dungeon);
    uint16_t getDungeonHeight(dungeon_t *dungeon);
    char getDungeonPoint(dungeon_t *dungeon,
                         point_t    point);
    point_t getDungeonSource(dungeon_t *dungeon);
    point_t getDungeonTarget(dungeon_t *dungeon);

    void setDungeonPoint(dungeon_t *dungeon,
                         point_t    point,
                         char       tile);

    void generateDungeon(dungeon_t *dungeon);

    bool isValidMove(dungeon_t *dungeon,
                     point_t    from,
                     point_t    to);

#endif
