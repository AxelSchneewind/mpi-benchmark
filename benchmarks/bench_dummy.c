#include "benchmarks/bench_template.h"

#include <mpi.h>

#include <assert.h>

// struct for data required while running benchmark
struct dummy_bench_state {
    int foo;
};

static int dummy_init(TestCase *test_case, Result *result, int comm_rank, void* s) {
    struct dummy_bench_state* state = (struct dummy_bench_state*) s;
    return 0;
}

static int dummy_cleanup(TestCase *test_case, Result *result, int comm_rank, void* s) {
    struct dummy_bench_state* state = (struct dummy_bench_state*) s;
    return 0;
}

static int dummy_start(TestCase *test_case, Result *result, int comm_rank, void* s) {
    return 0;
}

static int dummy_complete(TestCase *test_case, Result *result, int comm_rank, void* s) {
    struct dummy_bench_state* state = (struct dummy_bench_state*) s;
    return 0;
}

static int dummy_recv_partition_operation(TestCase *test_case, Result *result, int comm_rank, int partition, int threadnum, void* s) {
    struct dummy_bench_state* state = (struct dummy_bench_state*) s;
    assert (0 != comm_rank);
    return 0;
}

static int dummy_send_partition_operation(TestCase *test_case, Result *result, int comm_rank, int partition, int threadnum, void* s) {
    struct dummy_bench_state* state = (struct dummy_bench_state*) s;
    assert (0 == comm_rank);
    return 0;
}


// description of benchmarking function (i.e. the functions corresponding to transfer/benchmarking steps)
static const struct benchmarking_function dummy = {
    .state_size = sizeof(struct dummy_bench_state),
    .init = &dummy_init,
    .start = &dummy_start,
    .partition_operation_send = &dummy_send_partition_operation,
    .partition_operation_recv = &dummy_recv_partition_operation,
    .complete = &dummy_complete,
    .cleanup = &dummy_cleanup
};


// actual function
void bench_dummy(TestCase *test_case, Result *result, int comm_rank)
{ 
    execute(test_case, result, comm_rank, dummy);
}

