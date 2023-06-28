#include <chrono>
#include <dpu>
#include <iostream>
#include <random>
#include <unistd.h>

using namespace dpu;

#ifndef N_DPUS
#define N_DPUS 10
#endif

#define NUM_PATHS 100
#define RANGE_X 32
#define RANGE_Y 32
#define RANGE_Z 3

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

void 
create_bach(DpuSet &system, std::vector<std::vector<int>> &bach);

int
main(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    double total_copy_time = 0;
    double total_time = 0;

    try
    {
        auto system = DpuSet::allocate(N_DPUS);

        system.load("./Labyrinth_Multi_DPU/bin/dpu");

        std::vector<std::vector<int>> bach(system.dpus().size(), 
                                           std::vector<int>(6 * NUM_PATHS));
        std::vector<std::vector<grid_t>> grids(system.dpus().size(), 
                                               std::vector<grid_t>(1));

        create_bach(system, bach);

        // for (unsigned i = 0; i < system.dpus().size(); ++i)
        // {
        //     for (int j = 0; j < (6 * NUM_PATHS); j += 6)
        //     {
        //         std::cout << "(" << bach[i][j] << "," << bach[i][j + 1] << "," << bach[i][j + 2] << ") ->";
        //         std::cout << " (" << bach[i][j + 3] << "," << bach[i][j + 4] << "," << bach[i][j + 5] << ")" << std::endl;
        //     }
        //     std::cout << "-------------------" << std::endl;
        // }
        
        auto start = std::chrono::steady_clock::now();
    
        system.copy("bach", bach);

        auto end_copy = std::chrono::steady_clock::now();

        system.exec();

        auto start_copy_back = std::chrono::steady_clock::now();
    
        system.copy(grids, "grid");

        auto end = std::chrono::steady_clock::now();

        // std::cout << "-----------------------" << std::endl;
        // system.log(std::cout);


        total_copy_time += std::chrono::duration_cast<std::chrono::microseconds>(end_copy - start).count();
        total_copy_time += std::chrono::duration_cast<std::chrono::microseconds>(end - start_copy_back).count();
        total_time += std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

        std::cout << NR_TASKLETS << "\t"
                  << NUM_PATHS * 2 << "\t"
                  << N_DPUS << "\t"
                  << total_copy_time << "\t"
                  << total_time << std::endl;
    }
    catch (const DpuError &e)
    {
        std::cerr << e.what() << std::endl;
    }
    
    return 0;
}

void 
create_bach(DpuSet &system, std::vector<std::vector<int>> &bach)
{
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> rand_x_y(0, RANGE_X - 1);
    std::uniform_int_distribution<std::mt19937::result_type> rand_z(0, RANGE_Z - 1);
    int index;

    for (unsigned i = 0; i < system.dpus().size(); ++i)
    {
        for (int j = 0; j < NUM_PATHS; ++j)
        {
            index = j * 6;

            bach[i][index] = rand_x_y(rng);
            bach[i][index + 1] = rand_x_y(rng);
            bach[i][index + 2] = rand_z(rng);

            do
            {
                bach[i][index + 3] = rand_x_y(rng);
                bach[i][index + 4] = rand_x_y(rng);
                bach[i][index + 5] = rand_z(rng);
            } 
            while (bach[i][index] == bach[i][index + 3] &&
                   bach[i][index + 1] == bach[i][index + 4] &&
                   bach[i][index + 2] == bach[i][index + 5]);
        }
    }
}
