#include "benchmarks/bench_template.h"
#include "custom_psend.h"

#include <stdio.h>
#include <assert.h>

struct psend_custom_bench_state {
    custom_MPI_Request request;
    MPI_Status status;
};


static int psend_custom_init(TestCase *test_case, Result *result, int comm_rank, void* s) {
    struct psend_custom_bench_state* state = (struct psend_custom_bench_state*) s;
    if (0 == comm_rank) {
        MPI_CHECK(custom_MPI_Psend_init(test_case->buffer, test_case->buffer_size / test_case->partition_size, test_case->partition_size, MPI_CHAR, 1, 0, MPI_COMM_WORLD, MPI_INFO_ENV, &state->request));
    } else {
        MPI_CHECK(custom_MPI_Precv_init(test_case->buffer, test_case->buffer_size / test_case->partition_size_recv, test_case->partition_size_recv, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_INFO_ENV, &state->request));
    }
    return 0;
}

static int psend_custom_cleanup(TestCase *test_case, Result *result, int comm_rank, void* s) {
    struct psend_custom_bench_state* state = (struct psend_custom_bench_state*) s;
    MPI_CHECK(custom_MPI_Free(&state->request));
    return 0;
}

static int psend_custom_start(TestCase *test_case, Result *result, int comm_rank, void* s) {
    struct psend_custom_bench_state* state = (struct psend_custom_bench_state*) s;
    MPI_CHECK(custom_MPI_Start(state->request));
    return 0;
}


static int psend_custom_complete(TestCase *test_case, Result *result, int comm_rank, void* s) {
    struct psend_custom_bench_state* state = (struct psend_custom_bench_state*) s;
    MPI_CHECK(custom_MPI_Wait(state->request, MPI_STATUSES_IGNORE));
    return 0;
}

static int psend_custom_recv_partition_operation(TestCase *test_case, Result *result, int comm_rank, int partition, int threadnum, void* s) {
    return 0;
}

static int psend_custom_parrived_partition_operation(TestCase *test_case, Result *result, int comm_rank, int partition, int threadnum, void* s) {
    struct psend_custom_bench_state* state = (struct psend_custom_bench_state*) s;
    assert (1 == comm_rank);
    int flag = 0;
    MPI_CHECK(custom_MPI_Parrived(state->request, partition, &flag));
    return 0;
}

static int psend_custom_test_send_partition_operation(TestCase *test_case, Result *result, int comm_rank, int partition, int threadnum, void* s) {
    struct psend_custom_bench_state* state = (struct psend_custom_bench_state*) s;
    assert (0 == comm_rank);
    int flag;
    MPI_CHECK(custom_MPI_Pready(partition, state->request));
    MPI_CHECK(custom_MPI_Request_get_status(state->request, &flag, MPI_STATUS_IGNORE));
    return 0;
}

static int psend_custom_send_partition_operation(TestCase *test_case, Result *result, int comm_rank, int partition, int threadnum, void* s) {
    struct psend_custom_bench_state* state = (struct psend_custom_bench_state*) s;
    assert (0 == comm_rank);
    MPI_CHECK(custom_MPI_Pready(partition, state->request));
    return 0;
}




static const struct benchmarking_function psend_custom_parrived = {
    .state_size = sizeof(struct psend_custom_bench_state),
    .init = &psend_custom_init,
    .start = &psend_custom_start,
    .partition_operation_send = &psend_custom_send_partition_operation,
    .partition_operation_recv = &psend_custom_parrived_partition_operation,
    .complete = &psend_custom_complete,
    .cleanup = &psend_custom_cleanup
};

static const struct benchmarking_function psend_custom_test = {
    .state_size = sizeof(struct psend_custom_bench_state),
    .init = &psend_custom_init,
    .start = &psend_custom_start,
    .partition_operation_send = &psend_custom_test_send_partition_operation,
    .partition_operation_recv = &psend_custom_recv_partition_operation,
    .complete = &psend_custom_complete,
    .cleanup = &psend_custom_cleanup
};

static const struct benchmarking_function psend_custom = {
    .state_size = sizeof(struct psend_custom_bench_state),
    .init = &psend_custom_init,
    .start = &psend_custom_start,
    .partition_operation_send = &psend_custom_send_partition_operation,
    .partition_operation_recv = &psend_custom_recv_partition_operation,
    .complete = &psend_custom_complete,
    .cleanup = &psend_custom_cleanup
};

void bench_custom_psend_test(TestCase *test_case, Result *result, int comm_rank)
{ 
    execute(test_case, result, comm_rank, psend_custom_test);
}

void bench_custom_psend_parrived(TestCase *test_case, Result *result, int comm_rank)
{ 
    execute(test_case, result, comm_rank, psend_custom_parrived);
}

void bench_custom_psend(TestCase *test_case, Result *result, int comm_rank)
{ 
    execute(test_case, result, comm_rank, psend_custom);
}
