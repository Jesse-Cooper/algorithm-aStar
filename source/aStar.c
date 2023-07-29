#include "aStar.h"

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

#include "dataStructs/skipPQ.h"


// integer approximation of moving cost cardinally = 1 and diagonally = sqrt(2)
static const int COST_CARDINAL = 70;
static const int COST_DIAGONAL = 99;

// 8 directional movement system
static const point_t MOVES[] = {
    { 0, -1},  // north
    { 1, -1},  // north-east
    { 1,  0},  // east
    { 1,  1},  // south-east
    { 0,  1},  // south
    {-1,  1},  // south-west
    {-1,  0},  // west
    {-1, -1}   // north-west
};

static const int N_MOVES = sizeof(MOVES) / sizeof(point_t);


typedef struct pointData_s pointData_t;


struct pointData_s {
    point_t prev;
    uint32_t gScore;
    bool isClosed;
};


static pointData_t **initPointData(uint16_t width,
                                   uint16_t height);

static void freePointData(pointData_t **pointData,
                          uint16_t      width);

static void exploreNeighbours(dungeon_t    *dungeon,
                              skipPQ_t     *open,
                              pointData_t **pointData,
                              point_t       current,
                              point_t       target);

static uint16_t cost(point_t move);

static point_t *reconstructPath(pointData_t **pointData,
                                point_t       source,
                                point_t       target);


/* ------------------------------ START PUBLIC ------------------------------ */


/*
@context
    * Finds shortest path from `source` and `target` in `dungeon` if possible.
    * Uses the A* algorithm with an Octile distance heuristic.
        * The heuristic will never overestimate the actual path cost making it
          admissible.

@parameters
    * dungeon
        * Dungeon describing the layout to traverse and find path in.
    * source
        * Location to start from.
    * target
        * Location to find from `source`.

@return
    * Shortest path (sequence of points) from `source` to `target`.
        * `source` is not included.
        * `target` is included as the last point.
    * `NULL` if no path is possible.
*/
point_t *findPath(dungeon_t *dungeon,
                  point_t    source,
                  point_t    target)
{
    skipPQ_t *open;
    pointData_t **pointData;
    point_t *path;
    point_t current;

    open = initSkipPQ();
    pointData = initPointData(getDungeonWidth(dungeon),
                              getDungeonHeight(dungeon));
    path = NULL;

    // add `source` to `open` - will be the first node explored
    pointData[source.x][source.y].gScore = 0;
    initSkipNode(open, source, 0);

    // search for `target` or until no more points to explore
    while (!isSkipPQEmpty(open))
    {
        // grab next point based on the lowest f-score
        current = getSkipNodeData(getMinSkipNode(open));
        freeMinSkipNode(open);

        // if `current` already seen then skip it
        if (pointData[current.x][current.y].isClosed)
        {
            continue;
        }

        pointData[current.x][current.y].isClosed = true;

        // path found - reconstruct path
        if (isEqualPoints(current, target))
        {
            path = reconstructPath(pointData, source, target);
            break;
        }

        // explore all neighbouring points around `current`
        exploreNeighbours(dungeon, open, pointData, current, target);
    }

    freeSkipPQ(open);
    freePointData(pointData, getDungeonWidth(dungeon));

    return path;
}


/* ------------------------------- END PUBLIC ------------------------------- */
/* ----------------------------- START  PRIVATE ----------------------------- */


/*
@context
    * Initialises a 2D status array of each point within a dungeon.

@parameters
    * width
        * Width of dungeon.
    * height
        * Height of dungeon.

@return
    * 2D status array of each point within a dungeon.
*/
static pointData_t **initPointData(uint16_t width,
                                   uint16_t height)
{
    uint16_t x, y;
    pointData_t **pointData;

    pointData = malloc(sizeof(pointData_t*) * width);
    assert(pointData != NULL);

    for (x = 0; x < width; x += 1)
    {
        pointData[x] = malloc(sizeof(pointData_t) * height);
        assert(pointData[x] != NULL);

        for (y = 0; y < height; y += 1)
        {
            // g-score starts at the max distance possible
            pointData[x][y].gScore = UINT32_MAX;
            pointData[x][y].isClosed = false;
        }
    }

    return pointData;
}


/*
@context
    * Frees a 2D status array of each point within a dungeon.

@parameters
    * pointData
        * 2D status array to free.
    * width
        * Width of the dungeon - also the width of the array.
*/
static void freePointData(pointData_t **pointData,
                          uint16_t      width)
{
    uint16_t x;

    for (x = 0; x < width; x += 1)
    {
        free(pointData[x]);
    }
    free(pointData);
}


/*
@context
    * Explores the 8 neighbouring points around `current`.
    * Only valid neighbours are explored.
    * Once explored (g/h/f-score found) neighbours are added to `open` to later
      be expanded.

@parameters
    * dungeon
        * Dungeon describing the layout to traverse and find path in.
    * open
        * Priority queue to add neighbouring points to explore later.
        * The priority is a point's f-score.
    * pointData
        * 2D status array of each point within `dungeon`.
    * current
        * Location to expand neighbours around.
    * target
        * Location to find from `current`.
*/
static void exploreNeighbours(dungeon_t    *dungeon,
                              skipPQ_t     *open,
                              pointData_t **pointData,
                              point_t       current,
                              point_t       target)
{
    uint8_t i;
    uint32_t gScore, hScore, fScore;
    point_t neighbour;

    // explore neighbouring points around `current`
    for (i = 0; i < N_MOVES; i += 1)
    {
        neighbour = addPoints(current, MOVES[i]);

        // skip invalid moves
        if (!isValidMove(dungeon, current, neighbour))
        {
            continue;
        }

        gScore = pointData[current.x][current.y].gScore + cost(MOVES[i]);
        hScore = distancePoints(current, target, COST_CARDINAL, COST_DIAGONAL);
        fScore = gScore + hScore;

        // check if this is the new shortest path to `neighbour` from the source
        if (gScore < pointData[neighbour.x][neighbour.y].gScore)
        {
            pointData[neighbour.x][neighbour.y].prev = current;
            pointData[neighbour.x][neighbour.y].gScore = gScore;

            // add `neighbour` to `open` if not closed (expanded its neighbours)
            if (!pointData[neighbour.x][neighbour.y].isClosed)
            {
                initSkipNode(open, neighbour, fScore);
            }
        }
    }
}


/*
@context
    * Determine the cost of a `move`.

@parameters
    * move
        * Point indicating a unit move in an 8 directional movement system.
        * Moves whose combined x and y have a magnitude of 1 are cardinal.

@return
    * Cost of the `move`.
*/
static uint16_t cost(point_t move)
{
    // only moving in x or y direction not both
    if (abs(move.x + move.y) == 1)
    {
        return COST_CARDINAL;
    }
    return COST_DIAGONAL;
}


/*
@context
    * Creates an array of points to represent a found path.
    * Assumes path to `target` from `source` has been found.
        * Function only called once a path has been found.

@parameters
    * pointData
        * 2D status array of each point within a dungeon.
    * source
        * Location to stop reconstructing path when moving backwards.
    * target
        * Location to start reconstructing path when moving backwards.

@return
    * Shortest path (sequence of points) from `source` to `target`.
*/
static point_t *reconstructPath(pointData_t **pointData,
                                point_t       source,
                                point_t       target)
{
    point_t *path;
    point_t current;
    uint16_t length;
    int32_t i;

    // find length of `path`
    length = 0;
    current = target;
    while (!isEqualPoints(current, source))
    {
        length += 1;
        current = pointData[current.x][current.y].prev;
    }

    path = malloc(sizeof(point_t) * length);
    assert(path != NULL);

    // reconstruct `path` in reverse (from `target` to `source`)
    current = target;
    for (i = length - 1; i >= 0; i -= 1)
    {
        path[i] = current;
        current = pointData[current.x][current.y].prev;
    }

    return path;
}


/* ------------------------------ END  PRIVATE ------------------------------ */
