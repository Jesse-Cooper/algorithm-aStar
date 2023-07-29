#include "dungeon.h"

#include <assert.h>
#include <stdlib.h>
#include <math.h>


// safe zone around dungeon map never drawn on
static const int BORDER = 1;

// number of additional points between source and target
static const int POINTS_MIN = 1;
static const int POINTS_MAX = 3;

// radius of circle to draw at each step between 2 points
static const int RADIUS_MIN = 2;
static const int RADIUS_MAX = 3;

// minimum space between source and target
static const int SOURCE_TARGET_SEP = 22;

// either width or height must be big enough for `SOURCE_TARGET_SEP`
static const int MIN_SIZE = 2 * (SOURCE_TARGET_SEP + BORDER + RADIUS_MAX) + 3;

// character representations of dungeon tiles
static const char TILE_WALL = '#';
static const char TILE_FLOOR = ' ';
static const char TILE_SOURCE = '@';
static const char TILE_TARGET = 'X';

// distance between tiles in 8 directional movement (Chebyshev distance)
static const int COST = 1;


struct dungeon_s {
    char **map;

    uint16_t width;
    uint16_t height;

    uint8_t nPoints;
    point_t *points;
};


static uint16_t randInt(uint16_t min,
                        uint16_t max);

static void fillMap(dungeon_t *dungeon);

static void generatePoints(dungeon_t *dungeon);
static point_t generatePoint(uint16_t width,
                             uint16_t height);

static void connectPoints(dungeon_t *dungeon);
static void drawLine(char    **map,
                     point_t   start,
                     point_t   end,
                     uint8_t   radius);
static void drawCircle(char    **map,
                       point_t   centre,
                       uint8_t   radius);


/* ------------------------------ START PUBLIC ------------------------------ */


/*
@context
    * Initialises a dungeon with a random configuration.
    * Dungeon maps are organised in columns (`map[x][y]`).

@parameters
    * width
        * Width of dungeon map.
        * Must be `>= MIN_SIZE` if `height` is not.
    * height
        * Height of dungeon map.
        * Must be `>= MIN_SIZE` if `width` is not.

@return
    * Dungeon with a random configuration.
*/
dungeon_t *initDungeon(uint16_t width,
                       uint16_t height)
{
    uint16_t x;
    dungeon_t *dungeon;

    assert(width >= MIN_SIZE || height >= MIN_SIZE);

    dungeon = malloc(sizeof(dungeon_t));
    assert(dungeon != NULL);

    // initialise `dungeon` map to the given size
    dungeon->map = malloc(sizeof(char*) * width);
    assert(dungeon->map != NULL);
    for (x = 0; x < width; x += 1)
    {
        dungeon->map[x] = malloc(sizeof(char) * height);
        assert(dungeon->map[x] != NULL);
    }

    dungeon->width = width;
    dungeon->height = height;

    dungeon->nPoints = 0;
    dungeon->points = NULL;

    generateDungeon(dungeon);

    return dungeon;
}


/*
@context
    * Frees `dungeon`.

@parameters
    * dungeon
        * Dungeon to free.
*/
void freeDungeon(dungeon_t *dungeon)
{
    uint16_t x;

    for (x = 0; x < dungeon->width; x += 1)
    {
        free(dungeon->map[x]);
    }
    free(dungeon->map);

    free(dungeon->points);
    free(dungeon);
}


/*
@context
    * Gets the width of `dungeon`.

@parameters
    * dungeon
        * Dungeon to get width of.

@return
    * Width of `dungeon`.
*/
uint16_t getDungeonWidth(dungeon_t *dungeon)
{
    return dungeon->width;
}


/*
@context
    * Gets the height of `dungeon`.

@parameters
    * dungeon
        * Dungeon to get height of.

@return
    * Height of `dungeon`.
*/
uint16_t getDungeonHeight(dungeon_t *dungeon)
{
    return dungeon->height;
}


/*
@context
    * Gets tile character representation at `point` of `dungeon`.

@parameters
    * dungeon
        * Dungeon to get tile at `point`.
    * point
        * Location of tile to get.
        * Assumes `point` is within `dungeon` bounds.

@return
    * Tile character representation.
*/
char getDungeonPoint(dungeon_t *dungeon,
                     point_t    point)
{
    assert(point.x >= 0 && point.x < dungeon->width);
    assert(point.y >= 0 && point.y < dungeon->height);
    return dungeon->map[point.x][point.y];
}


/*
@context
    * Gets source point of `dungeon`.

@parameters
    * dungeon
        * Dungeon to get source point of.

@return
    * Location point of `dungeon` source.
*/
point_t getDungeonSource(dungeon_t *dungeon)
{
    return dungeon->points[0];
}


/*
@context
    * Gets target point of `dungeon`.

@parameters
    * dungeon
        * Dungeon to get target point of.

@return
    * Location point of `dungeon` target.
*/
point_t getDungeonTarget(dungeon_t *dungeon)
{
    return dungeon->points[dungeon->nPoints - 1];
}


/*
@context
    * Sets tile character representation at `point` of `dungeon`.

@parameters
    * dungeon
        * Dungeon to set tile of at `point`.
    * point
        * Location of tile to set.
        * Assumes `point` is within `dungeon` bounds.
    * tile
        * Character representation to set tile to.
*/
void setDungeonPoint(dungeon_t *dungeon,
                     point_t    point,
                     char       tile)
{
    assert(point.x >= 0 && point.x < dungeon->width);
    assert(point.y >= 0 && point.y < dungeon->height);
    dungeon->map[point.x][point.y] = tile;
}


/*
@context
    * Generates a new random configuration of `dungeon`.

@parameters
    * dungeon
        * Dungeon to generate map for.
*/
void generateDungeon(dungeon_t *dungeon)
{
    point_t source, target;

    fillMap(dungeon);

    // draw `dungeon` map by drawing lines between points
    generatePoints(dungeon);
    connectPoints(dungeon);

    source = getDungeonSource(dungeon);
    target = getDungeonTarget(dungeon);

    dungeon->map[source.x][source.y] = TILE_SOURCE;
    dungeon->map[target.x][target.y] = TILE_TARGET;
}


/*
@context
    * Checks if moving between 2 points in `dungeon` is valid.
    * Points must be adjacent in 8 directional movement (`distance = 1`).

@parameters
    * dungeon
        * Dungeon to check if valid move in.
    * from
        * Location moving from in `dungeon`.
        * Assumes `from` is valid within `dungeon`.
    * to
        * Location moving to in `dungeon`.

@return
    * Indication if moving between `from` and `to` in `dungeon` is valid.
*/
bool isValidMove(dungeon_t *dungeon,
                 point_t    from,
                 point_t    to)
{
    assert(from.x >= 0 && from.x < dungeon->width);
    assert(from.y >= 0 && from.y < dungeon->height);

    return (distancePoints(from, to, COST, COST) == 1

        // cannot move out of map bounds
        && (to.x >= 0 && to.x < dungeon->width)
        && (to.y >= 0 && to.y < dungeon->height)

        // cannot move into a wall
        && dungeon->map[to.x][to.y] != TILE_WALL

        // diagonal movement cannot clip wall (cannot move around corners)
        && dungeon->map[from.x][to.y] != TILE_WALL
        && dungeon->map[to.x][from.y] != TILE_WALL);
}


/* ------------------------------- END PUBLIC ------------------------------- */
/* ----------------------------- START  PRIVATE ----------------------------- */


/*
@context
    * Generates a uniform number between `min` and `max` (inclusive).

@parameters
    * min
        * Lower bounds (included).
    * max
        * Upper bounds (included).

@return
    * Random number between `min` and `max` (inclusive).
*/
static uint16_t randInt(uint16_t min,
                        uint16_t max)
{
    assert(min <= max);
    return rand() % (max + 1 - min) + min;
}


/*
@context
    * Fills `dungeon` map with wall tiles.
    * Wall tiles are later cut out by floor tiles.

@parameters
    * dungeon
        * Dungeon to fill with wall tiles.
*/
static void fillMap(dungeon_t *dungeon)
{
    uint16_t x, y;

    for (x = 0; x < dungeon->width; x += 1)
    {
        for (y = 0; y < dungeon->height; y += 1)
        {
            dungeon->map[x][y] = TILE_WALL;
        }
    }
}


/*
@context
    * Generates a sequence of points for `dungeon`.

@parameters
    * dungeon
        * Dungeon to generate points for.
*/
static void generatePoints(dungeon_t *dungeon)
{
    uint8_t i;
    point_t source, target;

    // free points from previous generation
    if (dungeon->points != NULL)
    {
        free(dungeon->points);
    }

    // determine number of points - source and target always included (+2)
    dungeon->nPoints = randInt(POINTS_MIN, POINTS_MAX) + 2;
    dungeon->points = malloc(sizeof(point_t) * dungeon->nPoints);
    assert(dungeon->points != NULL);

    // randomly place all but target (placed below)
    for (i = 0; i < dungeon->nPoints - 1; i += 1)
    {
        dungeon->points[i] = generatePoint(dungeon->width, dungeon->height);
    }

    // place target a minimum distance from source
    source = dungeon->points[0];
    target = generatePoint(dungeon->width, dungeon->height);
    while (distancePoints(target, source, COST, COST) < SOURCE_TARGET_SEP)
    {
        target = generatePoint(dungeon->width, dungeon->height);
    }
    dungeon->points[dungeon->nPoints - 1] = target;
}


/*
@context
    * Generates a single point.
    * Points are within dungeon boundaries accounting for circle cut outs.

@parameters
    * width
        * Width of map.
    * height
        * Height of map.

@return
    * A single random point.
*/
static point_t generatePoint(uint16_t width,
                             uint16_t height)
{
    uint16_t x, y;

    x = randInt(RADIUS_MAX + BORDER - 1, width - BORDER - RADIUS_MAX);
    y = randInt(RADIUS_MAX + BORDER - 1, height - BORDER - RADIUS_MAX);
    return initPoint(x, y);
}


/*
@context
    * Sequentially connect `dungeon` points by drawing lines between them.
        * Points are connected to adjacent points in `dungeon` point array.
    * Drawing lines is what gives `dungeon` its shape.

@parameters
    * dungeon
        * Dungeon to connect points within.
*/
static void connectPoints(dungeon_t *dungeon)
{
    uint8_t i, radius;

    // draw lines of varying sizes between adjacent points
    for (i = 0; i < dungeon->nPoints - 1; i += 1)
    {
        radius = randInt(RADIUS_MIN, RADIUS_MAX);
        drawLine(dungeon->map,
                 dungeon->points[i],
                 dungeon->points[i + 1],
                 radius);
    }
}


/*
@context
    * Draws a line between 2 points.
    * Uses Bresenham's line algorithm.
    * Assumes line and circles won't be out of bounds.
        * Ensured during point generation.

@parameters
    * map
        * Dungeon map to draw line in.
    * start
        * Location to start drawing line.
    * end
        * Location to stop drawing line.
    * radius
        * Radius of circle to draw at each step of line.
*/
static void drawLine(char    **map,
                     point_t   start,
                     point_t   end,
                     uint8_t   radius)
{
    int32_t dx, dy, error;
    int8_t sx, sy;
    point_t current;

    // distance between `start` and `end`
    dx = abs(start.x - end.x);
    dy = -abs(start.y - end.y);

    // step direction between `start` and `end`
    sx = start.x < end.x ? 1 : -1;
    sy = start.y < end.y ? 1 : -1;

    // incremental error to decide which direction to move next
    error = dx + dy;

    // repeatedly move 1 step and draw a circle from `start` until at `end`
    current = start;
    while (!isEqualPoints(current, end))
    {
        drawCircle(map, current, radius);

        // moves in either the x or y direction (not both at the same time)
        if (2 * error >= dy && current.x != end.x)
        {
            error += dy;
            current.x += sx;
        }
        else if (2 * error <= dx && current.y != end.y)
        {
            error += dx;
            current.y += sy;
        }
    }

    // draw last circle at `end` point
    drawCircle(map, current, radius);
}


/*
@context
    * Draws a circle centred on `centre`.
    * Assumes circle won't be out of bounds.
        * Ensured during point generation.

@parameters
    * map
        * Dungeon map to draw circle in.
    * centre
        * Centre location to draw circle.
        * The `radius` includes `centre`.
    * radius
        * Radius of circle.
*/
static void drawCircle(char    **map,
                       point_t   centre,
                       uint8_t   radius)
{
    int32_t x, y;
    int8_t halfHeight;

    // draw circle column wise
    for (x = -radius; x <= radius; x += 1)
    {
        // find column height/2 and draw it
        halfHeight = round(sqrt(radius * radius - x * x)) - 1;
        for (y = -halfHeight; y <= halfHeight; y += 1)
        {
            map[centre.x + x][centre.y + y] = TILE_FLOOR;
        }
    }
}


/* ------------------------------ END  PRIVATE ------------------------------ */
