#include "benchmarks/bench_template.h"
#include "benchmarks/bench_psend.h"

#include <stdio.h>
#include <assert.h>


static const struct benchmarking_function psend_parrived = {
    .state_size = sizeof(struct psend_bench_state),
    .init = &psend_init,
    .start = &psend_start,
    .partition_operation_send = &psend_send_partition_operation,
    .partition_operation_recv = &psend_parrived_partition_operation,
    .complete = &psend_complete,
    .cleanup = &psend_cleanup
};

static const struct benchmarking_function psend_test = {
    .state_size = sizeof(struct psend_bench_state),
    .init = &psend_init,
    .start = &psend_start,
    .partition_operation_send = &psend_test_send_partition_operation,
    .partition_operation_recv = &psend_recv_partition_operation,
    .complete = &psend_complete,
    .cleanup = &psend_cleanup
};

static const struct benchmarking_function psend = {
    .state_size = sizeof(struct psend_bench_state),
    .init = &psend_init,
    .start = &psend_start,
    .partition_operation_send = &psend_send_partition_operation,
    .partition_operation_recv = &psend_recv_partition_operation,
    .complete = &psend_complete,
    .cleanup = &psend_cleanup
};


void bench_psend_progress(TestCase *test_case, Result *result, int comm_rank)
{ 
    execute(test_case, result, comm_rank, psend_test);
}

void bench_psend_parrived(TestCase *test_case, Result *result, int comm_rank)
{ 
    execute(test_case, result, comm_rank, psend_parrived);
}

void bench_psend(TestCase *test_case, Result *result, int comm_rank)
{ 
    execute(test_case, result, comm_rank, psend);
}

