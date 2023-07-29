/*
@context
    * Provides a priority queue data structure.
    * The priority queue holds data by a priority.
        * Sorted in ascending order where 0 is the minimum.
        * Inserting data with priority equal to another node is inserted behind
          it.
    * Can only get the minimum node and must be removed to get the next node.
    * Underlying structure uses a skip list.
        * As a skip list is probabilistic, setting seed in `stdlib` may change
          the result.
*/


#ifndef _SKIP_PQ_H
    #define _SKIP_PQ_H

    #include <stdbool.h>
    #include <stdint.h>

    #include "../dataTypes/point.h"


    typedef struct skipNode_s skipNode_t;
    typedef struct skipPQ_s skipPQ_t;


    skipPQ_t *initSkipPQ();

    void initSkipNode(skipPQ_t *pq,
                      point_t   data,
                      uint32_t  priority);

    void freeSkipPQ(skipPQ_t *pq);
    void freeMinSkipNode(skipPQ_t *pq);

    bool isSkipPQEmpty(skipPQ_t *pq);

    skipNode_t *getMinSkipNode(skipPQ_t *pq);
    point_t getSkipNodeData(skipNode_t *node);
    uint32_t getSkipNodePriority(skipNode_t *node);

#endif
