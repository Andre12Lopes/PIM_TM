#include <assert.h>
#include <barrier.h>
#include <defs.h>
#include <perfcounter.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <tm.h>
#include <random.h>

#include "linked_list.h"

BARRIER_INIT(barr, NR_TASKLETS);

#include "metrics.h"

#ifndef
#define UPDATE_PERCENTAGE   0
#endif

#ifndef
#define SET_INITIAL_SIZE    10
#endif

#ifndef
#define RAND_RANGE          100
#endif

#define N_TRANSACTIONS      100

__mram_ptr intset_t *set;

Tx __mram_noinit tx_mram[NR_TASKLETS];

void test(TYPE Tx *tx, __mram_ptr intset_t *set, uint64_t *seed, int *last);

int main()
{
    TYPE Tx *tx;
    int val; 
    int tid;
    uint64_t seed;
    int i = 0;
    int last = -1;
    perfcounter_t initial_time;

    seed = me();
    tid = me();

#ifndef TX_IN_MRAM
    Tx tx_wram;
    tx = &tx_wram;
#else
    tx = &tx_mram[tid]
#endif

    TM_INIT(tx, tid);

    if (tid == 0)
    {
        set = set_new(INIT_SET_PARAMETERS);

        while (i < SET_INITIAL_SIZE) 
        {
            val = (RAND_R_FNC(seed) % RAND_RANGE) + 1;
            if (set_add(NULL, set, val, 0))
            {
                i++;
            }            
        }

        n_trans = N_TRANSACTIONS * NR_TASKLETS;
        n_aborts = 0;

        initial_time = perfcounter_config(COUNT_CYCLES, false);
    }

    barrier_wait(&barr);

    for (int i = 0; i < N_TRANSACTIONS; ++i)
    {
        test(tx, set, &seed, &last);
    }

    barrier_wait(&barr);

    if (me() == 0)
    {
        nb_cycles = perfcounter_get() - initial_time;

        nb_process_cycles = 0;
        nb_commit_cycles = 0;
        nb_wasted_cycles = 0;
        nb_process_read_cycles = 0;
        nb_process_write_cycles = 0;
        nb_process_validation_cycles = 0;
        nb_commit_validation_cycles = 0;
    }

    for (int i = 0; i < NR_TASKLETS; ++i)
    {
        if (me() == i)
        {
#ifdef TX_IN_MRAM
            n_aborts += t_mram[tid].Aborts;

            nb_process_cycles += ((double) t_mram[tid].process_cycles / (N_TRANSACTIONS * NR_TASKLETS));
            nb_process_read_cycles += ((double) t_mram[tid].total_read_cycles / (N_TRANSACTIONS * NR_TASKLETS));
            nb_process_write_cycles += ((double) t_mram[tid].total_write_cycles / (N_TRANSACTIONS * NR_TASKLETS));
            nb_process_validation_cycles += ((double) t_mram[tid].total_validation_cycles / (N_TRANSACTIONS * NR_TASKLETS));

            nb_commit_cycles += ((double) t_mram[tid].commit_cycles / (N_TRANSACTIONS * NR_TASKLETS));
            nb_commit_validation_cycles += ((double) t_mram[tid].total_commit_validation_cycles / (N_TRANSACTIONS * NR_TASKLETS));

            nb_wasted_cycles += ((double) (t_mram[tid].total_cycles - (t_mram[tid].process_cycles + t_mram[tid].commit_cycles)) / (N_TRANSACTIONS * NR_TASKLETS));
#else
            n_aborts += tx.Aborts;

            nb_process_cycles += ((double) tx.process_cycles / (N_TRANSACTIONS * NR_TASKLETS));
            nb_process_read_cycles += ((double) tx.total_read_cycles / (N_TRANSACTIONS * NR_TASKLETS));
            nb_process_write_cycles += ((double) tx.total_write_cycles / (N_TRANSACTIONS * NR_TASKLETS));
            nb_process_validation_cycles += ((double) tx.total_validation_cycles / (N_TRANSACTIONS * NR_TASKLETS));

            nb_commit_cycles += ((double) tx.commit_cycles / (N_TRANSACTIONS * NR_TASKLETS));
            nb_commit_validation_cycles += ((double) tx.total_commit_validation_cycles / (N_TRANSACTIONS * NR_TASKLETS));

            nb_wasted_cycles += ((double) (tx.total_cycles - (tx.process_cycles + tx.commit_cycles)) / (N_TRANSACTIONS * NR_TASKLETS));
#endif
        }

        barrier_wait(&barr);
    }

    return 0;
}


void test(TYPE Tx *tx, __mram_ptr intset_t *set, uint64_t *seed, int *last)
{
    int val, op;

    op = RAND_R_FNC(*seed) % 100;
    if (op < UPDATE_PERCENTAGE)
    {
        if (*last < 0)
        {
            /* Add random value */
            val = (RAND_R_FNC(*seed) % RAND_RANGE) + 1;
            if (set_add(tx, set, val, 1))
            {
                *last = val;
            }
        }
        else
        {
            /* Remove last value */
            set_remove(tx, set, *last);
            *last = -1;
        }
    }
    else
    {
        /* Look for random value */
        val = (RAND_R_FNC(*seed) % RAND_RANGE) + 1;
        set_contains(tx, set, val);
        // if (set_contains(tx, set, val))
        // {
        //     printf("FOUND!!\n");
        // }
        // else
        // {
        //     printf("NOT FOUND!!\n");
        // }
    }
}
