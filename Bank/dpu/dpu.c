#include <assert.h>
#include <barrier.h>
#include <defs.h>
#include <mram.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <random.h>
#include <tm.h>

BARRIER_INIT(my_barrier, NR_TASKLETS);

#define TRANSFER 2
#define ACCOUNT_V 100000
#define N_TRANSACTIONS 100000

#ifndef N_ACCOUNTS
#define N_ACCOUNTS 800
#endif

#include "metrics.h"

typedef struct
{
    uint32_t number;
    uint32_t balance;
} account_t;

#ifdef DATA_IN_MRAM
account_t __mram_noinit bank[N_ACCOUNTS];
#else
account_t bank[N_ACCOUNTS];
#endif

#ifdef TX_IN_MRAM
Tx __mram_noinit tx_mram[NR_TASKLETS];
#endif

void
initialize_accounts();
void
check_total();

int
main()
{
    TYPE Tx *tx;
    int tid;
    uint32_t ra, rb, rc, rd;
    uint32_t a, b, c, d;
    uint64_t s;

    s = (uint64_t)me();
    tid = me();

#ifndef TX_IN_MRAM
    Tx tx_wram;
    tx = &tx_wram;
#else
    tx = &tx_mram[tid]
#endif

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

        a = TM_LOAD(tx, &bank[ra].balance);
        a -= TRANSFER;
        TM_STORE(tx, &bank[ra].balance, a);

        b = TM_LOAD(tx, &bank[rb].balance);
        b += TRANSFER;
        TM_STORE(tx, &bank[rb].balance, b);

        c = TM_LOAD(tx, &bank[rc].balance);
        c -= TRANSFER;
        TM_STORE(tx, &bank[rc].balance, c);

        d = TM_LOAD(tx, &bank[rd].balance);
        d += TRANSFER;
        TM_STORE(tx, &bank[rd].balance, d);

        TM_COMMIT(tx);
    }

    get_metrics(tx, tid);

    check_total();

    return 0;
}

void
initialize_accounts()
{
    if (me() == 0)
    {
        for (int i = 0; i < N_ACCOUNTS; ++i)
        {

            bank[i].number = i + 1;
            bank[i].balance = ACCOUNT_V;
        }
    }
}

void
check_total()
{
    if (me() == 0)
    {
        printf("[");
        unsigned int total = 0;
        for (int i = 0; i < N_ACCOUNTS; ++i)
        {
            printf("%u -> %u | ", bank[i].number, bank[i].balance);
            total += bank[i].balance;
        }
        printf("]\n");

        printf("TOTAL = %u\n", total);

        assert(total == (N_ACCOUNTS * ACCOUNT_V));
    }
}
