#ifndef _MAZE_H_
#define _MAZE_H_

#include <string.h>

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

typedef struct path
{
    coordinate_t src;
    coordinate_t dest;
} path_t;

typedef struct maze
{
    path_t paths[NUM_PATHS];
} maze_t;

void
grid_alloc(__mram_ptr grid_t *grid)
{
    grid->height = RANGE_X;
    grid->width = RANGE_Y;
    grid->depth = RANGE_Z;

    memset(grid->points, GRID_POINT_EMPTY, sizeof(grid->points));
}

void
grid_copy(__mram_ptr grid_t *dstGridPtr, __mram_ptr grid_t *srcGridPtr)
{
    assert(srcGridPtr->height == dstGridPtr->height);
    assert(srcGridPtr->width == dstGridPtr->width);
    assert(srcGridPtr->depth == dstGridPtr->depth);

    memcpy(dstGridPtr->points, srcGridPtr->points, sizeof(dstGridPtr->points));
}

__mram_ptr grid_point_t *
grid_get_point_ref(__mram_ptr grid_t *gridPtr, long x, long y, long z)
{
    return &(gridPtr->points[(z * gridPtr->height * gridPtr->width) +
                             ((x * gridPtr->width) + y)]);
}

void
grid_set_point(__mram_ptr grid_t *gridPtr, long x, long y, long z, int value)
{
    grid_point_t point = { .value = value, .padding = 0 };
    mram_write(&point, grid_get_point_ref(gridPtr, x, y, z), sizeof(grid_point_t));
}

void
grid_get_point_indices(__mram_ptr grid_t *gridPtr, __mram_ptr grid_point_t *gridPointPtr,
                       long *xPtr, long *yPtr, long *zPtr)
{
    long area = gridPtr->height * gridPtr->width;
    long index = (gridPointPtr - gridPtr->points);
    (*zPtr) = index / area;
    (*yPtr) = index % gridPtr->width;
    (*xPtr) = (index / gridPtr->height) % gridPtr->height;
}

int
grid_get_point(__mram_ptr grid_t *gridPtr, long x, long y, long z)
{
    grid_point_t point = *(grid_get_point_ref(gridPtr, x, y, z));
    return point.value;
}

bool_t
grid_is_point_empty(__mram_ptr grid_t *gridPtr, long x, long y, long z)
{
    __mram_ptr grid_point_t *point = grid_get_point_ref(gridPtr, x, y, z);
    return ((point->value == GRID_POINT_EMPTY) ? TRUE : FALSE);
}

bool_t
grid_is_point_full(__mram_ptr grid_t *gridPtr, long x, long y, long z)
{
    grid_point_t point = *(grid_get_point_ref(gridPtr, x, y, z));
    return ((point.value == GRID_POINT_FULL) ? TRUE : FALSE);
}

bool_t
grid_is_point_valid(__mram_ptr grid_t *gridPtr, long x, long y, long z)
{
    if (x < 0 || x >= gridPtr->height || y < 0 || y >= gridPtr->width || z < 0 ||
        z >= gridPtr->depth)
    {
        return FALSE;
    }

    return TRUE;
}

void
grid_print(__mram_ptr grid_t *gridPtr)
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

void
grid_print_addr(__mram_ptr grid_t *gridPtr)
{
    for (int z = 0; z < gridPtr->depth; ++z)
    {
        printf("[z = %d]\n", z);
        for (int x = 0; x < gridPtr->width; ++x)
        {
            for (int y = 0; y < gridPtr->height; ++y)
            {
                printf("  %p", grid_get_point_ref(gridPtr, x, y, z));
            }
            puts("");
        }
    }
}

void
maze_read(__mram_ptr maze_t *maze, __mram_ptr grid_t *grid)
{
    for (long i = 0; i < NUM_PATHS; ++i)
    {
        if (!grid_is_point_valid(grid, PATHS[i][0], PATHS[i][1], PATHS[i][2]) ||
            !grid_is_point_valid(grid, PATHS[i][3], PATHS[i][4], PATHS[i][5]))
        {
            printf("[Error] Invalid pint\n");
            assert(1);
        }

        maze->paths[i].src.x = PATHS[i][0];
        maze->paths[i].src.y = PATHS[i][1];
        maze->paths[i].src.z = PATHS[i][2];

        maze->paths[i].dest.x = PATHS[i][3];
        maze->paths[i].dest.y = PATHS[i][4];
        maze->paths[i].dest.z = PATHS[i][5];

        // grid_set_point(grid, PATHS[i][0], PATHS[i][1], PATHS[i][2], GRID_POINT_FULL);
        // grid_set_point(grid, PATHS[i][3], PATHS[i][4], PATHS[i][5], GRID_POINT_FULL);

        grid_get_point_ref(grid, maze->paths[i].src.x, maze->paths[i].src.y, 
                           maze->paths[i].src.z)->value = GRID_POINT_FULL;
        grid_get_point_ref(grid, maze->paths[i].dest.x, maze->paths[i].dest.y, 
                           maze->paths[i].dest.z)->value = GRID_POINT_FULL;
    }
}

#endif /* _MAZE_H_ */
