/*
@context
    * Demonstrates the A* algorithm.
    * Creates random dungeon configurations and finds the shortest path between
      its source and target.
*/


#include <stdlib.h>

#include "aStar.h"
#include "interface.h"
#include "dataStructs/dungeon.h"
#include "dataTypes/point.h"


static const char KEY_QUIT = 'q';
static const int SEED = 7907;
static const char TILE_PATH = '.';


static void play(dungeon_t *dungeon);
static void displayDungeon(dungeon_t *dungeon);


/*
@context
    * Entry point of program.

@return
    * Indicates program successfully terminates.
*/
int main()
{
    dungeon_t *dungeon;

    srand(SEED);

    // enter main loop of program until user exit
    dungeon = initDungeon(WIDTH_CANVAS, HEIGHT_CANVAS);
    play(dungeon);
    freeDungeon(dungeon);

    return EXIT_SUCCESS;
}


/*
@context
     * Initialises the interface and displays different `dungeon` configurations
      until exited.
    * Will not initialise or will exited if terminal is too small for the
      interface.

@parameters
    * dungeon
        * Dungeon to display different configurations with their source to
          target path.
*/
static void play(dungeon_t *dungeon)
{
    char input;

    // initialise interface (terminal must be large enough)
    if (!initInterface())
    {
        return;
    }

    // keep displaying different `dungeon` configurations until exited
    input = ' ';
    while (input != KEY_QUIT)
    {
        // exit the interface if terminal becomes to small
        if (!isTerminalValidSize())
        {
            freeInterface();
            return;
        }

        // find the path and display the current `dungeon` configuration
        displayDungeon(dungeon);

        input = getInput();
        generateDungeon(dungeon);
    }

    freeInterface();
}


/*
@context
    * Finds the solution (source to target) of `dungeon` and display on the
      interface.

@parameters
    * dungeon
        * Dungeon to find solution of and display.
*/
static void displayDungeon(dungeon_t *dungeon)
{
    uint16_t i, x, y;
    point_t *path;
    point_t point;

    // find shortest path between the source and target in `dungeon`
    path = findPath(dungeon,
                    getDungeonSource(dungeon),
                    getDungeonTarget(dungeon));

    // draw the found path on `dungeon`
    i = 0;
    while (!isEqualPoints(path[i], getDungeonTarget(dungeon)))
    {
        setDungeonPoint(dungeon, path[i], TILE_PATH);
        i += 1;
    }

    // display `dungeon` with its `path`
    for (x = 0; x < getDungeonWidth(dungeon); x += 1)
    {
        for (y = 0; y < getDungeonHeight(dungeon); y += 1)
        {
            point = initPoint(x, y);
            updateCanvas(point, getDungeonPoint(dungeon, point));
        }
    }

    free(path);
}
