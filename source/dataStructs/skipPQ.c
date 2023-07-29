#include "skipPQ.h"

#include <assert.h>
#include <stdlib.h>


static const double PROB = 0.5;
static const int NEXT = 0;


struct skipNode_s
{
    skipNode_t **forward;
    uint8_t level;

    uint32_t priority;

    point_t data;
};

struct skipPQ_s
{
    skipNode_t *head;
};


static uint8_t randLevel();

static skipNode_t *initNode(point_t  data,
                            uint32_t priority,
                            uint8_t  level);
static void connectNode(skipPQ_t   *pq,
                        skipNode_t *node);

static void updatePQLevel(skipPQ_t *pq,
                          uint8_t   newLevel);


/* ------------------------------ START PUBLIC ------------------------------ */


/*
@context
    * Initialises an empty skip priority queue.
    * An empty node is inserted to act as the head of the queue.
        * Its level is constantly updated to be equal to the highest level.
        * It has the lowest priority so it is always at the top of the queue.
        * It is skipped over when getting the min node.

@return
    * Empty skip priority queue.
*/
skipPQ_t *initSkipPQ()
{
    skipPQ_t *pq;

    pq = malloc(sizeof(skipPQ_t));
    assert(pq != NULL);

    // head node has no data and has minimum priority (0)
    pq->head = initNode(initPoint(0, 0), 0, 1);
    pq->head->forward[NEXT] = NULL;

    return pq;
}


/*
@context
    * Inserts `data` into `pq` by its `priority`.

@parameters
    * pq
        * Skip priority queue to insert `data` into.
    * data
        * Data to insert into `pq`.
    * priority
        * Sort value of `data`.
        * Sorted in ascending order - `0` is the lowest.
*/
void initSkipNode(skipPQ_t *pq,
                  point_t   data,
                  uint32_t  priority)
{
    uint8_t level;
    skipNode_t *node;

    level = randLevel();
    node = initNode(data, priority, level);

    // increase `pq` head node level to match `level` if higher than it
    if (level > pq->head->level)
    {
        updatePQLevel(pq, level);
    }

    connectNode(pq, node);
}


/*
@context
    * Frees all allocated memory of `pq`.

@parameters
    * pq
        * Skip priority queue to free.
*/
void freeSkipPQ(skipPQ_t *pq)
{
    skipNode_t *prev;
    skipNode_t *current;

    // linearly traverse `pq` and free all nodes
    current = pq->head;
    while (current != NULL)
    {
        prev = current;
        current = current->forward[NEXT];

        free(prev->forward);
        free(prev);
    }

    free(pq);
}


/*
@context
    * Frees the minimum priority node from `pq`.
    * Nodes are sorted in ascending order by their priority - `0` is the lowest.
    * Minimum priority node is the second node of `pq` after the head.

@parameters
    * pq
        * Skip priority queue to free minimum priority node from.
*/
void freeMinSkipNode(skipPQ_t *pq)
{
    uint8_t i;
    uint8_t newLevel;
    skipNode_t *node;

    newLevel = pq->head->level;
    node = getMinSkipNode(pq);

    // disconnect `node` from `pq` and point head node to nodes after `node`
    for (i = 0; i < node->level; i += 1)
    {
        pq->head->forward[i] = node->forward[i];

        // stop if there are no more nodes above level `i` after `node`
        if (pq->head->forward[i] == NULL)
        {
            newLevel = i + 1;
            break;
        }
    }

    // reduce `pq` head node level if top levels no longer point to a node
    if (newLevel < pq->head->level)
    {
        updatePQLevel(pq, newLevel);
    }

    free(node->forward);
    free(node);
}


/*
@context
    * Determines if `pq` is empty.
    * Does not include the head node.

@parameters
    * pq
        * Skip priority queue to check if empty.

@return
    * Indicates if `pq` is empty.
*/
bool isSkipPQEmpty(skipPQ_t *pq)
{
    return pq->head->forward[NEXT] == NULL;
}


/*
@context
    * Gets minimum priority node from `pq`.
    * Minimum priority node is the second node of `pq` after the head.

@parameters
    * pq
        * Skip priority queue to get minimum priority node from.

@return
    * Minimum priority node of `pq`.
    * If `pq` is empty `NULL` is returned.
*/
skipNode_t *getMinSkipNode(skipPQ_t *pq)
{
    return pq->head->forward[NEXT];
}


/*
@context
    * Gets data held by `node`.

@parameters
    * node
        * Node to get data of.

@return
    * Data held by `node`.
*/
point_t getSkipNodeData(skipNode_t *node)
{
    return node->data;
}


/*
@context
    * Gets priority of `node`.

@parameters
    * node
        * Node to get priority of.

@return
    * Priority of `node`.
*/
uint32_t getSkipNodePriority(skipNode_t *node)
{
    return node->priority;
}


/* ------------------------------- END PUBLIC ------------------------------- */
/* ----------------------------- START  PRIVATE ----------------------------- */


/*
@context
    * Chooses a random level for a node.
    * P(level = l) = 1 / (PROB^l).
    * Provides tree like structure of the skip priority queue.

@return
    * Random positive level.
*/
static uint8_t randLevel()
{
    uint8_t level;

    level = 1;
    while (rand() < RAND_MAX * PROB)
    {
        level += 1;
    }

    return level;
}


/*
@context
    * Initialises a node.

@parameters
    * data
        * Data held by node.
    * priority
        * Sort value of node.
        * Sorted in ascending order - `0` is the lowest.
    * level
        * Level of node (forward size).

@return
    * Node with given properties.
    * Not yet connected to a skip priority queue.
*/
static skipNode_t *initNode(point_t  data,
                            uint32_t priority,
                            uint8_t  level)
{
    skipNode_t *node;

    node = malloc(sizeof(skipNode_t));
    assert(node != NULL);

    // forward values are set later when `node` connected
    node->forward = malloc(sizeof(skipNode_t*) * level);
    assert(node->forward != NULL);

    node->level = level;
    node->priority = priority;
    node->data = data;

    return node;
}


/*
@context
    * Connects `node` to `pq` by its priority.
    * If priority equal to another node is inserted behind it.

@parameters
    * pq
        * Skip priority queue to connect `node` to.
    * node
        * Node to connect to `pq`.
*/
static void connectNode(skipPQ_t   *pq,
                        skipNode_t *node)
{
    int16_t i;
    skipNode_t *current;

    // traverse across `pq` stating at the top level and moving down to level 1
    current = pq->head;
    for (i = pq->head->level - 1; i >= 0; i -= 1)
    {
        // move across level `i` until directly after `node` priority position
        while (current->forward[i] != NULL
            && current->forward[i]->priority < node->priority)
        {
            current = current->forward[i];
        }

        // connect `node` to nodes directly before/after its priority position
        if (i < node->level)
        {
            node->forward[i] = current->forward[i];
            current->forward[i] = node;
        }
    }
}


/*
@context
    * Changes the `pq` head node level to `newLevel`.

@parameters
    * pq
        * Skip priority queue to change head node level to `newLevel`.
    * newLevel
        * New level of the `pq` head node.
*/
static void updatePQLevel(skipPQ_t *pq,
                          uint8_t   newLevel)
{
    uint8_t i;
    skipNode_t **newForward;

    newForward = malloc(sizeof(skipNode_t*) * newLevel);
    assert(newForward != NULL);

    // copy old forward skips to `newForward`
    for (i = 0; i < newLevel; i += 1)
    {
        // if `newForward` has less levels then the top levels are pruned
        if (i < pq->head->level)
        {
            newForward[i] = pq->head->forward[i];
        }
        // if `newForward` has extra levels they start at NULL
        else
        {
            newForward[i] = NULL;
        }
    }

    free(pq->head->forward);
    pq->head->forward = newForward;
    pq->head->level = newLevel;
}


/* ------------------------------ END  PRIVATE ------------------------------ */
