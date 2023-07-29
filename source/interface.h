/*
@context
    * Provides a text interface with an area (canvas) to display a dungeon.
        * Also allows key inputs to be received from the user.
    * The terminal must be >= than the interface dimensions.
*/


#ifndef _INTERFACE_H
    #define _INTERFACE_H

    #include <stdbool.h>

    #include "dataTypes/point.h"


    // size of the canvas within the interface to display the dungeon
    static const int WIDTH_CANVAS = 69;
    static const int HEIGHT_CANVAS = 16;


    bool initInterface();
    void freeInterface();

    char getInput();

    void updateCanvas(point_t point,
                      char    tile);

    bool isTerminalValidSize();

#endif
