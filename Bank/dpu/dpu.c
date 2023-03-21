#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <defs.h>
#include <mram.h>
#include <barrier.h>

#include <tm.h>
#include <random.h>

BARRIER_INIT(my_barrier, NR_TASKLETS);

#define TRANSFER        2
#define ACCOUNT_V       1000
#define N_TRANSACTIONS  1000

#ifndef N_ACCOUNTS
#define N_ACCOUNTS      800
#endif

#include "metrics.h"

#ifdef ACC_IN_MRAM
uint32_t __mram_noinit bank[N_ACCOUNTS];
#else
uint32_t bank[N_ACCOUNTS];
#endif

#ifdef TX_IN_MRAM
Tx __mram_noinit tx_mram[NR_TASKLETS];
#endif

void initialize_accounts();
void check_total();

int main()
{
    TYPE_ACC Tx *tx;
#ifndef TX_IN_MRAM
    Tx tx_wram;
    tx = &tx_wram;
#else
    tx = &tx_mram[tid]
#endif
    int tid;
    uint32_t ra, rb, rc, rd;
    uint32_t a, b, c, d;
    uint64_t s;
    
    s = (uint64_t)me();
    tid = me();

    TM_INIT(tx, tid);

    initialize_accounts();

    start_count(tid);

    for (int i = 0; i < N_TRANSACTIONS; ++i)
    {
        ra = RAND_R_FNC(s) % N_ACCOUNTS;
        rb = RAND_R_FNC(s) % N_ACCOUNTS;
        rc = RAND_R_FNC(s) % N_ACCOUNTS;
        rd = RAND_R_FNC(s) % N_ACCOUNTS;

        TM_START(tx);

        a = TM_LOAD(tx, &bank[ra]);
        a -= TRANSFER;
        TM_STORE(tx, &bank[ra], a);

        b = TM_LOAD(tx, &bank[rb]);
        b += TRANSFER;
        TM_STORE(tx, &bank[rb], b);

        c = TM_LOAD(tx, &bank[rc]);
        c -= TRANSFER;
        TM_STORE(tx, &bank[rc], c);

        d = TM_LOAD(tx, &bank[rd]);
        d += TRANSFER;
        TM_STORE(tx, &bank[rd], d);

        TM_COMMIT(tx);
    }

    get_metrics(tx, tid);

    check_total();
    
    return 0;
}

void initialize_accounts()
{
    if (me() == 0)
    {
        for (int i = 0; i < N_ACCOUNTS; ++i)
        {
            bank[i] = ACCOUNT_V;
        }    
    }
}

void check_total()
{
    if (me() == 0)
    {
        printf("[");
        unsigned int total = 0;
        for (int i = 0; i < N_ACCOUNTS; ++i)
        {
            printf("%d,", bank[i]);
            total += bank[i];
        }
        printf("]\n");

        printf("TOTAL = %u\n", total);

        assert(total == (N_ACCOUNTS * ACCOUNT_V));
    }
}
