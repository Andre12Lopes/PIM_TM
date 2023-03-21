#include <assert.h>
#include <stdlib.h>
#include <perfcounter.h>

#include "rw_locks.h"
#include "rw_locks_internal.h"

volatile stm_word_t lock_table[LOCK_ARRAY_SIZE];

void 
stm_init(TYPE stm_tx *tx, int tid)
{
    tx->rng = tid + 1;
    tx->process_cycles = 0;
    tx->read_cycles = 0;
    tx->write_cycles = 0;
    tx->validation_cycles = 0;
    tx->total_read_cycles = 0;
    tx->total_write_cycles = 0;
    tx->total_validation_cycles = 0;
    tx->total_commit_validation_cycles = 0;
    tx->commit_cycles = 0;
    tx->total_cycles = 0;
    tx->start_time = 0;
    tx->start_read = 0;
    tx->start_write = 0;
    tx->start_validation = 0;
    tx->retries = 0;
    tx->aborts = 0;
}

void 
stm_start(TYPE stm_tx *tx)
{
    return int_stm_start(tx);
}

stm_word_t 
stm_load(TYPE stm_tx *tx, TYPE_ACC stm_word_t *addr)
{
    return int_stm_load(tx, addr);
}

void 
stm_store(TYPE stm_tx *tx, TYPE_ACC stm_word_t *addr, stm_word_t value)
{
    int_stm_store(tx, addr, value);
}

int 
stm_commit(TYPE stm_tx *tx)
{
    return int_stm_commit(tx);
}