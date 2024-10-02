#include "benchmarks/bench_template.h"
#include "benchmarks/bench_psend.h"


#include <stdio.h>
#include <assert.h>


static const struct benchmarking_function psend_gpu_parrived = {
    .state_size = sizeof(struct psend_bench_state),
    .init = &psend_init,
    .start = &psend_start,
    .partition_operation_send = &psend_send_partition_operation,
    .partition_operation_recv = &psend_parrived_partition_operation,
    .complete = &psend_complete,
    .cleanup = &psend_cleanup
};

static const struct benchmarking_function psend_gpu_test = {
    .state_size = sizeof(struct psend_bench_state),
    .init = &psend_init,
    .start = &psend_start,
    .partition_operation_send = &psend_test_send_partition_operation,
    .partition_operation_recv = &psend_recv_partition_operation,
    .complete = &psend_complete,
    .cleanup = &psend_cleanup
};

static const struct benchmarking_function psend_gpu = {
    .state_size = sizeof(struct psend_bench_state),
    .init = &psend_init,
    .start = &psend_start,
    .partition_operation_send = &psend_send_partition_operation,
    .partition_operation_recv = &psend_recv_partition_operation,
    .complete = &psend_complete,
    .cleanup = &psend_cleanup
};



void bench_psend_gpu_progress(TestCase *test_case, Result *result, int comm_rank)
{ 
    execute_gpu(test_case, result, comm_rank, psend_gpu_test);
}

void bench_psend_gpu_parrived(TestCase *test_case, Result *result, int comm_rank)
{ 
    execute_gpu(test_case, result, comm_rank, psend_gpu_parrived);
}

void bench_psend_gpu(TestCase *test_case, Result *result, int comm_rank)
{ 
    execute_gpu(test_case, result, comm_rank, psend_gpu);
}


