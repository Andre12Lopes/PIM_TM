#include <assert.h>
#include <barrier.h>
#include <defs.h>
#include <limits.h>
#include <mram.h>
#include <perfcounter.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <random.h>
#include <tm.h>

#include "metrics.h"

BARRIER_INIT(labyrinth_barr, NR_TASKLETS);

#include "inputs/inputs-x48-y48-z3-n48.h"

#define PARAM_BENDCOST 1
#define PARAM_XCOST 1
#define PARAM_YCOST 1
#define PARAM_ZCOST 2

#include "types.h"
#include "vector.h"
#include "queue.h"
#include "maze.h"
#include "router.h"

#ifdef TX_IN_MRAM
Tx __mram_noinit tx_mram[NR_TASKLETS];
#endif

__mram_noinit router_t router;
__mram_noinit maze_t maze;
__mram_noinit grid_t grid;
__mram_noinit grid_t thread_local_grids[NR_TASKLETS];
// __mram_noinit list_t pathVectorList;

int
main()
{
    TYPE Tx *tx;
    uint64_t s;
    int tid;

    __dma_aligned coordinate_t src, dest;
    __mram_ptr grid_t *my_grid;
    __mram_ptr queue_t *myExpansionQueue;
    __mram_ptr vector_t *point_vector;

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

    start_count(tid);

    if (tid == 0)
    {
        maze_read(&maze);
        router_alloc(&router, PARAM_XCOST, PARAM_YCOST, PARAM_ZCOST, PARAM_BENDCOST);
        grid_alloc(&grid);

    }
    barrier_wait(&labyrinth_barr);

    myExpansionQueue = queue_alloc(-1);
    grid_alloc(my_grid);

    // for (int i = tid; i < NUM_PATHS; i += NR_TASKLETS)
    for (int i = tid; i < 1; i += NR_TASKLETS)
    {
        mram_read(&(maze.paths[i].src), &src, sizeof(coordinate_t));
        mram_read(&(maze.paths[i].dest), &dest, sizeof(coordinate_t));

        grid_copy(my_grid, &grid);
        if (pdo_expansion(&router, my_grid, myExpansionQueue, &src, &dest))
        {
            // point_vector = pdo_traceback(&grid, my_grid, &dest, PARAM_BENDCOST);

            // if (pointVectorPtr) 
            // {
            //     TMGRID_ADDPATH(&grid, pointVectorPtr);
            //     TM_LOCAL_WRITE(success, TRUE);
            // }
        }
    }

    // get_metrics(tx, tid, loop);

    return 0;
}
