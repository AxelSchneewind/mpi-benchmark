#include "benchmarks/bench.h"
#include "benchmarks/bench_template.h"

#include "stdio.h"


#define LOG(format)                                               \
    {                                                             \
        printf("(R%i) [%s:%i]: ", comm_rank, __FILE__, __LINE__); \
        printf(format);                                           \
        printf("\n");                                             \
    }
#define LOGF(format, args...)                                     \
    {                                                             \
        printf("(R%i) [%s:%i]: ", comm_rank, __FILE__, __LINE__); \
        ptest_srintf(format, args);                                     \
        printf("\n");                                             \
    }


struct win_single_bench_state {
    MPI_Win window;
};

static int win_single_init(TestCase *test_case, Result *result, int comm_rank, void* s) {
    struct win_single_bench_state* state = (struct win_single_bench_state*) s;

    MPI_CHECK(MPI_Win_create(test_case->buffer, test_case->buffer_size, 1, MPI_INFO_ENV, MPI_COMM_WORLD, &state->window));

    return 0;
}

static int win_single_cleanup(TestCase *test_case, Result *result, int comm_rank, void* s) {
    struct win_single_bench_state* state = (struct win_single_bench_state*) s;
    MPI_CHECK(MPI_Win_free(&state->window));
    return 0;
}

static int win_single_send_partition_operation(TestCase *test_case, Result *result, int comm_rank, int partition, int threadnum, void* s) {
    struct win_single_bench_state* state = (struct win_single_bench_state*) s;

    MPI_CHECK(MPI_Win_lock(MPI_LOCK_EXCLUSIVE, 1, MPI_MODE_NOCHECK, state->window));

    MPI_CHECK(MPI_Put(
        test_case->buffer + test_case->partition_size * partition,
        test_case->partition_size, MPI_BYTE, 1,
        test_case->partition_size * partition,
        test_case->partition_size, MPI_BYTE, state->window));

    MPI_CHECK(MPI_Win_unlock(1, state->window));

    return 0;
}


// description of benchmarking function (i.e. the functions corresponding to transfer/benchmarking steps)
static const struct benchmarking_function win_single = {
    .state_size = sizeof(struct win_single_bench_state),
    .init = &win_single_init,
    .start = NULL,
    .partition_operation_send = &win_single_send_partition_operation,
    .partition_operation_recv = NULL,
    .complete = NULL,
    .cleanup = &win_single_cleanup
};

// entry point
void bench_win_single(TestCase *test_case, Result *result, int comm_rank)
{ 
    execute(test_case, result, comm_rank, win_single);
}

