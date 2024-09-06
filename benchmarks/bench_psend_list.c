#include "benchmarks/bench_template.h"
#include "benchmarks/bench_psend.h"

#include <stdio.h>
#include <assert.h>

static int psendlist_send_partition_operation(TestCase *test_case, Result *result, int comm_rank, int partition, int threadnum, void* s) {
    struct psend_bench_state* state = (struct psend_bench_state*) s;
    assert (0 == comm_rank);
    if (0 != partition)
        MPI_CHECK(MPI_Pready_list(test_case->partitions_per_thread, permutation_at(test_case->send_pattern, threadnum * test_case->partitions_per_thread), state->request));
    return 0;
}

static const struct benchmarking_function psend_list = {
    .state_size = sizeof(struct psend_bench_state),
    .init = &psend_init,
    .start = &psend_start,
    .partition_operation_send = &psendlist_send_partition_operation,
    .partition_operation_recv = &psend_recv_partition_operation,
    .complete = &psend_complete,
    .cleanup = &psend_cleanup
};

void bench_psend_list(TestCase *test_case, Result *result, int comm_rank)
{ 
    execute(test_case, result, comm_rank, psend_list);
}


