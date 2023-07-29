#include "interface.h"

#include <ctype.h>
#include <ncurses.h>
#include <stdio.h>


// text interface - bottom area (canvas) is where the dungeon is displayed
static const char INTERFACE[] =
    "#######################################################################\n"
    "#                 A* Algorithm  (Dungeon Pathfinding)                 #\n"
    "#                                                                     #\n"
    "# Press Q to quit                                                     #\n"
    "# Press ANY KEY for a new dungeon configuration                       #\n"
    "# Keep the terminal size larger than 72x24                            #\n"
    "#######################################################################\n"
    "#                                                                     #\n"
    "#                                                                     #\n"
    "#                                                                     #\n"
    "#                                                                     #\n"
    "#                                                                     #\n"
    "#                                                                     #\n"
    "#                                                                     #\n"
    "#                                                                     #\n"
    "#                                                                     #\n"
    "#                                                                     #\n"
    "#                                                                     #\n"
    "#                                                                     #\n"
    "#                                                                     #\n"
    "#                                                                     #\n"
    "#                                                                     #\n"
    "#                                                                     #\n"
    "#######################################################################\n";

// size of the interface (terminal must be >= these dimensions)
static const int WIDTH_INTERFACE = 72;
static const int HEIGHT_INTERFACE = 24;

// location of canvas in the interface (top-left corner)
static const int X_CANVAS = 1;
static const int Y_CANVAS = 7;

static const char MSG_ERROR_SIZE[] = "Terminal size must be at least %dx%d\n";


/* ------------------------------ START PUBLIC ------------------------------ */


/*
@context
    * Initialises and displays the ncurses text interface.
    * Will not initialise if terminal too small.

@return
    * Indicates if interface successfully initialised.
*/
bool initInterface()
{
    initscr();

    // do not initialise interface if terminal too small
    if (!isTerminalValidSize())
    {
        freeInterface();
        return false;
    }

    // hide cursor and characters typed
    curs_set(0);
    noecho();

    // display a blank interface (canvas empty)
    printw(INTERFACE);
    refresh();

    return true;
}


/*
@context
    * Closes ncurses interface.
*/
void freeInterface()
{
    endwin();

    // exit was due to terminal being too small
    if (!isTerminalValidSize())
    {
        printf(MSG_ERROR_SIZE, WIDTH_INTERFACE, HEIGHT_INTERFACE);
    }
}


/*
@context
    * Gets a key input from user.
    * Program is paused until the input is received.

@return
    * Key input from user
    * If input is a letter it becomes lowercase.
*/
char getInput()
{
    return tolower(getch());
}


/*
@context
    * Updates the character of a single `point` of the canvas.
    * The bottom space of the `INTERFACE` is the canvas.

@parameters
    * point
        * Location to update canvas.
        * The canvas starts (0, 0) in its top-left.
    * tile
        * Character to update `point` to.
*/
void updateCanvas(point_t point,
                  char    tile)
{
    mvaddch(Y_CANVAS + point.y, X_CANVAS + point.x, tile);
    refresh();
}


/*
@context
    * Checks if the terminal is a valid size for the interface.

@return
    * Indicates if the terminal is a valid size for the interface.
*/
bool isTerminalValidSize()
{
    return (COLS >= WIDTH_INTERFACE) && (LINES >= HEIGHT_INTERFACE);
}


/* ------------------------------- END PUBLIC ------------------------------- */
