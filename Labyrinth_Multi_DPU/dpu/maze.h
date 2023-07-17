#ifndef _MAZE_H_
#define _MAZE_H_

#include <string.h>
#include <dpu_alloc.h>

enum
{
    GRID_POINT_FULL = -2L,
    GRID_POINT_EMPTY = -1L
};

typedef struct grid_point
{
    int value;      // point value
    int padding;    // 4 byte padding
} grid_point_t;

typedef struct grid
{
    long height;
    long width;
    long depth;
    grid_point_t points[RANGE_X * RANGE_Y * RANGE_Z];
} grid_t;

typedef struct coordinate
{
    long x;
    long y;
    long z;
} coordinate_t;

typedef struct pair 
{
    __mram_ptr void *firstPtr;
    __mram_ptr void *secondPtr;
} pair_t;


typedef struct path
{
    coordinate_t src;
    coordinate_t dest;
} path_t;

typedef struct maze
{
    // __mram_ptr queue_t *workQueuePtr;
    path_t paths[NUM_PATHS];
} maze_t;

void
grid_alloc(grid_t *grid)
{
    grid->height = RANGE_X;
    grid->width = RANGE_Y;
    grid->depth = RANGE_Z;

    memset(grid->points, GRID_POINT_EMPTY, sizeof(grid->points));
}

void
grid_copy(grid_t *dstGridPtr, grid_t *srcGridPtr)
{
    // assert(srcGridPtr->height == dstGridPtr->height);
    // assert(srcGridPtr->width == dstGridPtr->width);
    // assert(srcGridPtr->depth == dstGridPtr->depth);

    memcpy(dstGridPtr->points, srcGridPtr->points, sizeof(dstGridPtr->points));
}

grid_point_t *
grid_get_point_ref(grid_t *gridPtr, long x, long y, long z)
{
    return &(gridPtr->points[(z * gridPtr->height * gridPtr->width) +
                             ((x * gridPtr->width) + y)]);
}

void
grid_set_point(grid_t *gridPtr, long x, long y, long z, int value)
{
    // grid_point_t point = { .value = value, .padding = 0 };
    // mram_write(&point, grid_get_point_ref(gridPtr, x, y, z), sizeof(grid_point_t));
    grid_get_point_ref(gridPtr, x, y, z)->value = value;
}

void
grid_get_point_indices(grid_t *gridPtr, grid_point_t *gridPointPtr,
                       long *xPtr, long *yPtr, long *zPtr)
{
    long area = gridPtr->height * gridPtr->width;
    long index = (gridPointPtr - gridPtr->points);
    (*zPtr) = index / area;
    (*yPtr) = index % gridPtr->width;
    (*xPtr) = (index / gridPtr->height) % gridPtr->height;
}

// int
// grid_get_point(__mram_ptr grid_t *gridPtr, long x, long y, long z)
// {
//     grid_point_t point = *(grid_get_point_ref(gridPtr, x, y, z));
//     return point.value;
// }

bool_t
grid_is_point_empty(grid_t *gridPtr, long x, long y, long z)
{
    grid_point_t *point = grid_get_point_ref(gridPtr, x, y, z);
    return ((point->value == GRID_POINT_EMPTY) ? TRUE : FALSE);
}

bool_t
grid_is_point_full(grid_t *gridPtr, long x, long y, long z)
{
    grid_point_t point = *(grid_get_point_ref(gridPtr, x, y, z));
    return ((point.value == GRID_POINT_FULL) ? TRUE : FALSE);
}

bool_t
grid_is_point_valid(grid_t *gridPtr, long x, long y, long z)
{
    if (x < 0 || x >= gridPtr->height || y < 0 || y >= gridPtr->width || z < 0 ||
        z >= gridPtr->depth)
    {
        return FALSE;
    }

    return TRUE;
}

void
grid_print(grid_t *gridPtr)
{
    for (int z = 0; z < gridPtr->depth; ++z)
    {
        printf("[z = %d]\n", z);
        for (int x = 0; x < gridPtr->width; ++x)
        {
            for (int y = 0; y < gridPtr->height; ++y)
            {
                printf("%4d", grid_get_point_ref(gridPtr, x, y, z)->value);
            }
            puts("");
        }
    }
}

// void
// grid_print_addr(__mram_ptr grid_t *gridPtr)
// {
//     for (int z = 0; z < gridPtr->depth; ++z)
//     {
//         printf("[z = %d]\n", z);
//         for (int x = 0; x < gridPtr->width; ++x)
//         {
//             for (int y = 0; y < gridPtr->height; ++y)
//             {
//                 printf("  %p", grid_get_point_ref(gridPtr, x, y, z));
//             }
//             puts("");
//         }
//     }
// }

__mram_ptr coordinate_t *
coordinate_alloc(long x, long y, long z)
{
    __mram_ptr coordinate_t *coordinatePtr;

    coordinatePtr = (__mram_ptr coordinate_t *)mram_malloc(sizeof(coordinate_t));
    
    if (coordinatePtr) 
    {
        coordinatePtr->x = x;
        coordinatePtr->y = y;
        coordinatePtr->z = z;
    }

    return coordinatePtr;
}

__mram_ptr pair_t *
pair_alloc(__mram_ptr void *firstPtr, __mram_ptr void *secondPtr)
{
    __mram_ptr pair_t *pairPtr;

    pairPtr = (__mram_ptr pair_t *)mram_malloc(sizeof(pair_t));
    
    if (pairPtr != NULL) 
    {
        pairPtr->firstPtr = firstPtr;
        pairPtr->secondPtr = secondPtr;
    }

    return pairPtr;
}

// void
// maze_read(__mram_ptr maze_t *maze, __mram_ptr grid_t *grid, __mram_ptr int *paths)
// {
//     __mram_ptr grid_point_t *srcPoint;
//     __mram_ptr grid_point_t *dstPoint;

//     maze->workQueuePtr = queue_alloc(NUM_PATHS);
//     // assert(maze->workQueuePtr);

//     for (long i = 0; i < NUM_PATHS; ++i)
//     {
//         __mram_ptr coordinate_t *srcPtr = 
//             coordinate_alloc(paths[(i * 6)], paths[(i * 6) + 1], paths[(i * 6) + 2]);
//         __mram_ptr coordinate_t *dstPtr = 
//             coordinate_alloc(paths[(i * 6) + 3], paths[(i * 6) + 4], paths[(i * 6) + 5]);
     
//         __mram_ptr pair_t *coordinatePairPtr = pair_alloc(srcPtr, dstPtr);
     
//         queue_push(maze->workQueuePtr, (__mram_ptr void *)coordinatePairPtr);

//         srcPoint = grid_get_point_ref(grid, srcPtr->x, srcPtr->y, srcPtr->z);
//         dstPoint = grid_get_point_ref(grid, dstPtr->x, dstPtr->y, dstPtr->z);
	
// 	grid_point_t point = { .value = GRID_POINT_FULL, .padding = 0 };
	
// 	*srcPoint = point;
// 	*dstPoint = point;
//     }
// }

void
maze_read(__mram_ptr maze_t *maze, grid_t *grid, __mram_ptr int *paths)
{
    grid_point_t *srcPoint;
    grid_point_t *dstPoint;
    
    for (long i = 0; i < NUM_PATHS; ++i)
    {
        maze->paths[i].src.x = paths[(i * 6)];
        maze->paths[i].src.y = paths[(i * 6) + 1];
        maze->paths[i].src.z = paths[(i * 6) + 2];

        maze->paths[i].dest.x = paths[(i * 6) + 3];
        maze->paths[i].dest.y = paths[(i * 6) + 4];
        maze->paths[i].dest.z = paths[(i * 6) + 5];

	srcPoint = grid_get_point_ref(grid, maze->paths[i].src.x, maze->paths[i].src.y, maze->paths[i].src.z);
	dstPoint = grid_get_point_ref(grid, maze->paths[i].dest.x, maze->paths[i].dest.y, maze->paths[i].dest.z);

	grid_point_t point = { .value = GRID_POINT_FULL, .padding = 0 };

	*srcPoint = point;
	*dstPoint = point;
    }
}

#endif /* _MAZE_H_ */
