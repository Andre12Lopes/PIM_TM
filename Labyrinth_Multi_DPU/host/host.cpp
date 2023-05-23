#include <chrono>
#include <dpu>
#include <iostream>
#include <random>
#include <unistd.h>

using namespace dpu;

#define N_DPUS 10
#define PATHS_DPU 100

void 
create_bach(DpuSet &system, std::vector<std::vector<int>> &bach);

int
main(int argc, char **argv)
{
    // double total_copy_time = 0;
    // double total_time = 0;

    // try
    // {
        auto system = DpuSet::allocate(N_DPUS);

    //     system.load("./Labyrinth_Multi_DPU/bin/dpu");

        std::vector<std::vector<int>> bach(system.dpus().size(), 
                                           std::vector<int>(3 * PATHS_DPU));

    //     for (int i = 0; i < N_BACHES; ++i)
    //     {
    //         create_bach(system, bach);

    //         auto start = std::chrono::steady_clock::now();
        
    //         system.copy("bach", bach);

    //         auto end_copy = std::chrono::steady_clock::now();

    //         system.exec();

    //         auto end = std::chrono::steady_clock::now();

    //         total_copy_time += std::chrono::duration_cast<std::chrono::microseconds>(end_copy - start).count();
    //         total_time += std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    //     }

    //     auto dpu = system.dpus()[0];
    //     nThreads.front().resize(1);
    //     dpu->copy(nThreads, "n_tasklets");

    //     std::cout << (double) nThreads.front().front() << "\t"
    //               << N_TANSACTIONS << "\t"
    //               << N_BACHES << "\t"
    //               << N_DPUS << "\t"
    //               << total_copy_time << "\t"
    //               << total_time << std::endl;
    // }
    // catch (const DpuError &e)
    // {
    //     std::cerr << e.what() << std::endl;
    // }

    create_bach(system, bach);
    
    return 0;
}

void create_bach(DpuSet &system, std::vector<std::vector<int>> &bach)
{
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> rand_x_y(0, 32);
    std::uniform_int_distribution<std::mt19937::result_type> rand_z(0, 3);

    for (unsigned i = 0; i < system.dpus().size(); ++i)
    {
        // for (int j = 0; j < (3 * PATHS_DPU); ++j)
        // {
        //     bach[i][j] = rand(rng);
        // }
        std::cout << rand_z(rng) << std::endl;
    }
}
