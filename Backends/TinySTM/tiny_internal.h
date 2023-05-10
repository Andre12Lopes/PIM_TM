#ifndef _TINY_INTERNAL_H_
#define _TINY_INTERNAL_H_

#include <stdio.h>
#include <stdlib.h>

#include <perfcounter.h>

#include "atomic.h"
#include "utils.h"

#define LOCK_ARRAY_LOG_SIZE 10

#define OWNED_BITS 1       /* 1 bit */
#define INCARNATION_BITS 3 /* 3 bits */
#define LOCK_BITS (OWNED_BITS + INCARNATION_BITS)

#define WRITE_MASK 0x01
#define OWNED_MASK (WRITE_MASK)
#define INCARNATION_MAX ((1 << INCARNATION_BITS) - 1)
#define INCARNATION_MASK (INCARNATION_MAX << 1)

#define LOCK_GET_OWNED(l) (l & OWNED_MASK)
#define LOCK_GET_WRITE(l) (l & WRITE_MASK)
#define LOCK_SET_ADDR_WRITE(a) (a | WRITE_MASK)
#define LOCK_GET_ADDR(l) (l & ~(stm_word_t)OWNED_MASK)
#define LOCK_GET_TIMESTAMP(l) (l >> (LOCK_BITS))
#define LOCK_SET_TIMESTAMP(t) (t << (LOCK_BITS))
#define LOCK_GET_INCARNATION(l) ((l & INCARNATION_MASK) >> OWNED_BITS)
#define LOCK_SET_INCARNATION(i) (i << OWNED_BITS)
#define LOCK_UPD_INCARNATION(l, i)                                                       \
    ((l & ~(stm_word_t)(INCARNATION_MASK | OWNED_MASK)) | LOCK_SET_INCARNATION(i))

#define LOCK_ARRAY_SIZE (1 << LOCK_ARRAY_LOG_SIZE)
#define LOCK_MASK (LOCK_ARRAY_SIZE - 1)

#define LOCK_SHIFT_EXTRA 0
#define LOCK_SHIFT ((sizeof(stm_word_t) == 4 ? 2 : 3) + LOCK_SHIFT_EXTRA)
#define LOCK_IDX(a) (((stm_word_t)(a) >> LOCK_SHIFT) & LOCK_MASK)
#define GET_LOCK(a) (_tinystm.locks + LOCK_IDX(a))

#define CLOCK (_tinystm.gclock)
#define GET_CLOCK (ATOMIC_GET_CLOCK_VALUE(&CLOCK))
#define FETCH_INC_CLOCK (ATOMIC_FETCH_INC_FULL(&CLOCK, 1))

enum
{ /* Transaction status */
    TX_ACTIVE = 1,
    TX_COMMITTED = (1 << 1),
    TX_ABORTED = (2 << 1),
};

#define SET_STATUS(s, v) ((s) = (v))
#define UPDATE_STATUS(s, v) ((s) = (v))
#define GET_STATUS(s) (s)
#define IS_ACTIVE(s) ((GET_STATUS(s) & 0x01) == TX_ACTIVE)
#define IS_ABORTED(s) ((GET_STATUS(s) & 0x04) == TX_ABORTED)

typedef struct
{
    volatile stm_word_t locks[LOCK_ARRAY_SIZE];
    volatile stm_word_t gclock;
    unsigned int initialized;
} global_t;

extern global_t _tinystm;

static void
stm_rollback(TYPE stm_tx_t *tx, unsigned int reason);

/*
 * Check if stripe has been read previously.
 */
static inline TYPE r_entry_t *
stm_has_read(TYPE stm_tx_t *tx, volatile stm_word_t *lock)
{
    TYPE r_entry_t *r;

    /* Look for read */
    r = tx->r_set.entries;
    for (int i = tx->r_set.nb_entries; i > 0; i--, r++)
    {
        if (r->lock == lock)
        {
            return r;
        }
    }

    return NULL;
}

/*
 * Check if address has been written previously.
 */
static inline TYPE w_entry_t *
stm_has_written(TYPE stm_tx_t *tx, volatile TYPE_ACC stm_word_t *addr)
{
    TYPE w_entry_t *w;

    /* Look for write */
    w = tx->w_set.entries;
    for (int i = tx->w_set.nb_entries; i > 0; i--, w++)
    {
        if (w->addr == addr)
        {
            return w;
        }
    }

    return NULL;
}

// TODO change alocation to DPU
static void
stm_allocate_rs_entries(TYPE stm_tx_t *tx, int extend)
{
    (void)tx;
    (void)extend;
    
    assert(0);
}

/*
 * (Re)allocate write set entries.
 */
static void
stm_allocate_ws_entries(TYPE stm_tx_t *tx, int extend)
{
    (void)tx;
    (void)extend;

    assert(0);
}

#ifdef WRITE_BACK_CTL
#include "tiny_wbctl.h"
#endif

#ifdef WRITE_BACK_ETL
#include "tiny_wbetl.h"
#endif

#ifdef WRITE_THROUGH_ETL
#include "tiny_wtetl.h"
#endif

#include "backoff.h"

/*
 * Initialize the transaction descriptor before start or restart.
 */
static inline void
int_stm_prepare(TYPE stm_tx_t *tx)
{
    /* Read/write set */
    tx->w_set.has_writes = 0;
    tx->w_set.nb_acquired = 0;

    tx->r_set.nb_entries = 0;
    tx->w_set.nb_entries = 0;

    tx->r_set.size = R_SET_SIZE;
    tx->w_set.size = W_SET_SIZE;

    tx->read_only = 0;

    tx->read_cycles = 0;
    tx->write_cycles = 0;
    tx->validation_cycles = 0;

    /* Start timestamp */
    tx->start = tx->end = GET_CLOCK; /* OPT: Could be delayed until first read/write */
    if (tx->start == 0)
    {
    }

    /* Set status */
    UPDATE_STATUS(tx->status, TX_ACTIVE);
}

static inline void
int_stm_start(TYPE stm_tx_t *tx)
{
    /* Initialize transaction descriptor */
    if (tx->start_time == 0)
    {
        tx->start_time = perfcounter_config(COUNT_CYCLES, false);
    }
    tx->time = perfcounter_config(COUNT_CYCLES, false);

    int_stm_prepare(tx);
}

/*
 * Rollback transaction.
 */
static void
stm_rollback(TYPE stm_tx_t *tx, unsigned int reason)
{
    (void)reason;

    tx->abort_cycles += perfcounter_get() - tx->time;

#if defined(WRITE_BACK_CTL)
    stm_wbctl_rollback(tx);
#elif defined(WRITE_BACK_ETL)
    stm_wbetl_rollback(tx);
#elif defined(WRITE_THROUGH_ETL)
    stm_wtetl_rollback(tx);
#endif

    tx->aborts++;

#ifdef BACKOFF
    tx->retries++;
    if (tx->retries > 3)
    { /* TUNABLE */
        backoff(tx, tx->retries);
    }
#endif

    /* Set status to ABORTED */
    SET_STATUS(tx->status, TX_ABORTED);
}

static inline stm_word_t
int_stm_load(TYPE stm_tx_t *tx, volatile TYPE_ACC stm_word_t *addr)
{
    stm_word_t val = -1;

    tx->start_read = perfcounter_config(COUNT_CYCLES, false);

#if defined(WRITE_BACK_CTL)
    val = stm_wbctl_read(tx, addr);
#elif defined(WRITE_BACK_ETL)
    val = stm_wbetl_read(tx, addr);
#elif defined(WRITE_THROUGH_ETL)
    val = stm_wtetl_read(tx, addr);
#endif

    tx->read_cycles += perfcounter_get() - tx->start_read;

    return val;
}

static inline void
int_stm_store(TYPE stm_tx_t *tx, volatile TYPE_ACC stm_word_t *addr, stm_word_t value)
{
    tx->start_write = perfcounter_config(COUNT_CYCLES, false);

#if defined(WRITE_BACK_CTL)
    stm_wbctl_write(tx, addr, value, ~(stm_word_t)0);
#elif defined(WRITE_BACK_ETL)
    stm_wbetl_write(tx, addr, value, ~(stm_word_t)0);
#elif defined(WRITE_THROUGH_ETL)
    stm_wtetl_write(tx, addr, value, ~(stm_word_t)0);
#endif

    tx->write_cycles += perfcounter_get() - tx->start_write;
}

static inline int
int_stm_commit(TYPE stm_tx_t *tx)
{
    uint64_t t_process_cycles;

    t_process_cycles = perfcounter_get() - tx->time;
    tx->time = perfcounter_config(COUNT_CYCLES, false);

    /* A read-only transaction can commit immediately */
    if (tx->w_set.nb_entries != 0)
    {
#if defined(WRITE_BACK_CTL)
        stm_wbctl_commit(tx);
#elif defined(WRITE_BACK_ETL)
        stm_wbetl_commit(tx);
#elif defined(WRITE_THROUGH_ETL)
        stm_wtetl_commit(tx);
#endif
    }

    if (IS_ABORTED(tx->status))
    {
        return 0;
    }

    /* Set status to COMMITTED */
    SET_STATUS(tx->status, TX_COMMITTED);

    tx->process_cycles += t_process_cycles;
    tx->commit_cycles += perfcounter_get() - tx->time;
    tx->total_cycles += perfcounter_get() - tx->start_time;
    tx->total_read_cycles += tx->read_cycles;
    tx->total_write_cycles += tx->write_cycles;
    tx->total_validation_cycles += tx->validation_cycles;

    tx->start_time = 0;
    tx->retries = 0;

    return 1;
}

#endif /* _TINY_INTERNAL_H_ */
