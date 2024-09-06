#include "benchmarks/bench.h"
#include "benchmarks/bench_template.h"

#include <mpi.h>

struct win_bench_state {
    MPI_Win* windows;
};

static int win_init(TestCase *test_case, Result *result, int comm_rank, void* s) {
    struct win_bench_state* state = (struct win_bench_state*) s;

    state->windows = calloc(test_case->partition_count, sizeof(MPI_Win));

    for (int p = 0; p < test_case->partition_count; p++)
        MPI_CHECK(MPI_Win_create(test_case->buffer + p * test_case->partition_size, test_case->partition_size, 1, MPI_INFO_ENV, MPI_COMM_WORLD, &state->windows[p]));

    return 0;
}

static int win_cleanup(TestCase *test_case, Result *result, int comm_rank, void* s) {
    struct win_bench_state* state = (struct win_bench_state*) s;

    for (int p = 0; p < test_case->partition_count; p++)
        MPI_CHECK(MPI_Win_free(&state->windows[p]));

    free(state->windows);
    state->windows = NULL;

    return 0;
}

static int win_send_partition_operation(TestCase *test_case, Result *result, int comm_rank, int partition, int threadnum, void* s) {
    struct win_bench_state* state = (struct win_bench_state*) s;

    MPI_CHECK(MPI_Win_lock(MPI_LOCK_EXCLUSIVE, 1, MPI_MODE_NOCHECK, state->windows[partition]));

    MPI_CHECK(MPI_Put(
        test_case->buffer + test_case->partition_size * partition,
        test_case->partition_size, MPI_BYTE, 1, 0,
        test_case->partition_size, MPI_BYTE, state->windows[partition]));

    MPI_CHECK(MPI_Win_unlock(1, state->windows[partition]));

    return 0;
}


// description of benchmarking function (i.e. the functions corresponding to transfer/benchmarking steps)
static const struct benchmarking_function win = {
    .state_size = sizeof(struct win_bench_state),
    .init = &win_init,
    .start = NULL,
    .partition_operation_send = &win_send_partition_operation,
    .partition_operation_recv = NULL,
    .complete = NULL,
    .cleanup = &win_cleanup
};

// entry point
void bench_win(TestCase *test_case, Result *result, int comm_rank)
{ 
    execute(test_case, result, comm_rank, win);
}
