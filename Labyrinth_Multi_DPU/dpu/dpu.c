#include <assert.h>
#include <barrier.h>
#include <defs.h>
#include <limits.h>
#include <mram.h>
#include <perfcounter.h>
#include <stdint.h>
#include <stdlib.h>

#include <tm.h>

BARRIER_INIT(labyrinth_barr, NR_TASKLETS);

#define PARAM_BENDCOST 1
#define PARAM_XCOST 1
#define PARAM_YCOST 1
#define PARAM_ZCOST 2

#define NUM_PATHS 100
#define RANGE_X 16
#define RANGE_Y 16
#define RANGE_Z 3

// __mram int bach[NUM_PATHS * 6];
__mram int bach[NUM_PATHS * 6] = 
{
    12,13,0,8,15,1,
    9,15,1,6,4,1,
    4,3,2,8,4,1,
    3,2,2,10,15,2,
    3,11,1,10,6,2,
    15,14,2,8,1,2,
    0,2,2,12,0,2,
    15,10,0,10,2,0,
    7,7,0,14,2,0,
    10,15,0,9,9,2,
    3,10,2,6,9,1,
    2,12,1,7,9,0,
    6,5,0,8,15,0,
    2,4,0,1,2,2,
    12,8,2,7,6,2,
    13,8,1,15,11,0,
    10,3,1,10,6,0,
    0,8,0,7,11,0,
    10,13,0,3,4,2,
    7,1,2,2,0,0,
};

#include "types.h"
#include "vector.h"
#include "queue.h"
#include "maze.h"
#include "router.h"

#ifdef TX_IN_MRAM
Tx __mram_noinit tx_mram[NR_TASKLETS];
#endif

__mram router_t router;
__mram maze_t maze;
grid_t grid;
grid_t thread_local_grids[NR_TASKLETS];

int
main()
{
    TYPE Tx *tx;
    uint64_t s;
    int tid;

    __dma_aligned coordinate_t src, dest;
    grid_t *my_grid;
    __mram_ptr queue_t *myExpansionQueue;
    __mram_ptr vector_t *point_vector;
    bool_t success;
    long n;

    tid = me();
    s = (uint64_t)me();

    my_grid = &thread_local_grids[tid];

#ifndef TX_IN_MRAM
    Tx tx_wram;
    tx = &tx_wram;
#else
    tx = &tx_mram[tid];
#endif

    TM_INIT(tx, tid);

    if (tid == 0)
    {
        grid_alloc(&grid);
        maze_read(&maze, bach);
        // router_alloc(&router, PARAM_XCOST, PARAM_YCOST, PARAM_ZCOST, PARAM_BENDCOST);
        router.x_cost = PARAM_XCOST;
        router.y_cost = PARAM_YCOST;
        router.z_cost = PARAM_ZCOST;
        router.bend_cost = PARAM_BENDCOST;
    }
    barrier_wait(&labyrinth_barr);

    myExpansionQueue = queue_alloc(-1);

    grid_alloc(my_grid);

    for (int i = tid; i < NUM_PATHS; i += NR_TASKLETS)
    {
        mram_read(&(maze.paths[i].src), &src, sizeof(coordinate_t));
        mram_read(&(maze.paths[i].dest), &dest, sizeof(coordinate_t));

        success = FALSE;
        point_vector = NULL;

        TM_START(tx);

        grid_copy(my_grid, &grid);
        if (pdo_expansion(&router, my_grid, myExpansionQueue, &src, &dest))
        {
            point_vector = pdo_traceback(&grid, my_grid, &dest, PARAM_BENDCOST);
            
            if (point_vector)
            {
                // ================ ADD PATH TO GRID ====================
                n = vector_get_size(point_vector);
                
                for (long i = 1; i < (n - 1); i++)
                {
                    grid_point_t *gridPointPtr = 
                        (grid_point_t *)vector_at(point_vector, i);

                    int value = 
                        (int)TM_LOAD_LOOP(tx, (uintptr_t *)&(gridPointPtr->value));

                    if (value != GRID_POINT_EMPTY)
                    {
                        TM_RESTART_LOOP(tx);
                    }

                    TM_STORE_LOOP(tx, (uintptr_t *)&(gridPointPtr->value), GRID_POINT_FULL);
                }

                if (tx->status == 4)
                {
                    // We break out of previous loop because of an abort
                    continue;
                }
                else
                {
                    // Transaction did not abort
                    success = TRUE;                
                }  
                // ======================================================
            }
        }

        TM_COMMIT(tx);
    }

    uint64_t n_aborts = 0;
    for (int i = 0; i < NR_TASKLETS; ++i)
    {
        if (tid == i)
        {
            n_aborts += tx->aborts;
        }
    }

    if (tid == 0)
    {
        printf("> %f\n", ((double)n_aborts * 100) / (n_aborts + NUM_PATHS));
    }

    return 0;
}
