#pragma once

#include "benchmarks/bench_template.h"
#include "benchmarks/bench_psend.h"

#include <stdio.h>
#include <assert.h>

struct psend_bench_state {
    MPI_Request request;
    MPI_Status status;
};


static int psend_init(TestCase *test_case, Result *result, int comm_rank, void* s) {
    struct psend_bench_state* state = (struct psend_bench_state*) s;
    if (0 == comm_rank) {
        MPI_CHECK(MPI_Psend_init(test_case->buffer, test_case->buffer_size / test_case->partition_size, test_case->partition_size, MPI_CHAR, 1, 0, MPI_COMM_WORLD, MPI_INFO_ENV, &state->request));
    } else {
        MPI_CHECK(MPI_Precv_init(test_case->buffer, test_case->buffer_size / test_case->partition_size_recv, test_case->partition_size_recv, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_INFO_ENV, &state->request));
    }
    return 0;
}

static int psend_cleanup(TestCase *test_case, Result *result, int comm_rank, void* s) {
    struct psend_bench_state* state = (struct psend_bench_state*) s;
    MPI_CHECK(MPI_Request_free(&state->request));
    return 0;
}

static int psend_start(TestCase *test_case, Result *result, int comm_rank, void* s) {
    struct psend_bench_state* state = (struct psend_bench_state*) s;
    MPI_CHECK(MPI_Start(&state->request));
    return 0;
}


static int psend_complete(TestCase *test_case, Result *result, int comm_rank, void* s) {
    struct psend_bench_state* state = (struct psend_bench_state*) s;
    MPI_CHECK(MPI_Wait(&state->request, MPI_STATUSES_IGNORE));
    return 0;
}

static int psend_recv_partition_operation(TestCase *test_case, Result *result, int comm_rank, int partition, int threadnum, void* s) {
    return 0;
}

static int psend_parrived_partition_operation(TestCase *test_case, Result *result, int comm_rank, int partition, int threadnum, void* s) {
    struct psend_bench_state* state = (struct psend_bench_state*) s;
    assert (1 == comm_rank);
    int flag = 0;
    MPI_CHECK(MPI_Parrived(state->request, partition, &flag));
    return 0;
}

static int psend_send_partition_operation(TestCase *test_case, Result *result, int comm_rank, int partition, int threadnum, void* s) {
    struct psend_bench_state* state = (struct psend_bench_state*) s;
    assert (0 == comm_rank);
    int flag;
    MPI_CHECK(MPI_Pready(partition, state->request));
    return 0;
}

static int psendlist_send_partition_operation(TestCase *test_case, Result *result, int comm_rank, int partition, int threadnum, void* s) {
    struct psend_bench_state* state = (struct psend_bench_state*) s;
    assert (0 == comm_rank);
    if (0 != partition)
        MPI_CHECK(MPI_Pready_list(test_case->partitions_per_thread, permutation_at(test_case->send_pattern, threadnum * test_case->partitions_per_thread), state->request));
    return 0;
}



static int psend_test_send_partition_operation(TestCase *test_case, Result *result, int comm_rank, int partition, int threadnum, void* s) {
    struct psend_bench_state* state = (struct psend_bench_state*) s;
    assert (0 == comm_rank);
    int flag;
    MPI_CHECK(MPI_Pready(partition, state->request));
    MPI_CHECK(MPI_Request_get_status(state->request, &flag, MPI_STATUS_IGNORE));
    return 0;
}



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

static const struct benchmarking_function psend_list = {
    .state_size = sizeof(struct psend_bench_state),
    .init = &psend_init,
    .start = &psend_start,
    .partition_operation_send = &psendlist_send_partition_operation,
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
