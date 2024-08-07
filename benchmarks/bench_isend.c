#include "benchmarks/bench_template.h"

#include "benchmarks/bench.h"

#include "stdio.h"


struct isend_bench_state {
    int flag;
    MPI_Request *requests;
    MPI_Status *statuses;
};


static int isend_init(TestCase *test_case, Result *result, int comm_rank, void* s) {
    struct isend_bench_state* state = (struct isend_bench_state*) s;
    state->flag = 0;
    state->requests = calloc(sizeof(MPI_Request), test_case->partition_count);
    state->statuses = calloc(sizeof(MPI_Status), test_case->partition_count);
    return 0;
}

static int isend_cleanup(TestCase *test_case, Result *result, int comm_rank, void* s) {
    struct isend_bench_state* state = (struct isend_bench_state*) s;
    free(state->requests);
    free(state->statuses);
    return 0;
}

static int isend_start(TestCase *test_case, Result *result, int comm_rank, void* s) {
    struct isend_bench_state* state = (struct isend_bench_state*) s;
    return 0;
}


static int isend_complete(TestCase *test_case, Result *result, int comm_rank, void* s) {
    struct isend_bench_state* state = (struct isend_bench_state*) s;
    if (!state->flag)
        MPI_CHECK(MPI_Waitall(test_case->partition_count, state->requests, state->statuses));
    return 0;
}

static int isend_testall_complete(TestCase *test_case, Result *result, int comm_rank, void* s) {
    struct isend_bench_state* state = (struct isend_bench_state*) s;
    while (!state->flag)
        MPI_CHECK(MPI_Testall(test_case->partition_count, state->requests, &state->flag, state->statuses));
    return 0;
}



static int isend_testall_partition_operation(TestCase *test_case, Result *result, int comm_rank, int partition, int threadnum,  void* s) {
    struct isend_bench_state* state = (struct isend_bench_state*) s;
    if (0 == comm_rank) {
        MPI_CHECK(MPI_Isend(test_case->buffer + partition * test_case->partition_size, test_case->partition_size, MPI_BYTE, 1, partition, MPI_COMM_WORLD, &state->requests[partition]));
        if (!state->flag)
        MPI_CHECK(MPI_Testall(test_case->partition_count, state->requests, &state->flag, state->statuses));
    } else {
        MPI_CHECK(MPI_Irecv(test_case->buffer + partition * test_case->partition_size, test_case->partition_size, MPI_BYTE, 0, partition, MPI_COMM_WORLD, &state->requests[partition]));
    }
    return 0;
}

static int isend_test_partition_operation(TestCase *test_case, Result *result, int comm_rank, int partition, int threadnum,  void* s) {
    struct isend_bench_state* state = (struct isend_bench_state*) s;
    if (0 == comm_rank) {
        MPI_CHECK(MPI_Isend(test_case->buffer + partition * test_case->partition_size, test_case->partition_size, MPI_BYTE, 1, partition, MPI_COMM_WORLD, &state->requests[partition]));
        if (!state->flag)
            MPI_CHECK(MPI_Request_get_status(state->requests[partition], &state->flag, &result->send_status));
    } else {
        MPI_CHECK(MPI_Irecv(test_case->buffer + partition * test_case->partition_size, test_case->partition_size, MPI_BYTE, 0, partition, MPI_COMM_WORLD, &state->requests[partition]));
    }
    return 0;
}

static int isend_partition_operation(TestCase *test_case, Result *result, int comm_rank, int partition, int threadnum,  void* s) {
    struct isend_bench_state* state = (struct isend_bench_state*) s;
    if (0 == comm_rank) {
        MPI_CHECK(MPI_Isend(test_case->buffer + partition * test_case->partition_size, test_case->partition_size, MPI_BYTE, 1, partition, MPI_COMM_WORLD, &state->requests[partition]));
    } else {
        MPI_CHECK(MPI_Irecv(test_case->buffer + partition * test_case->partition_size, test_case->partition_size, MPI_BYTE, 0, partition, MPI_COMM_WORLD, &state->requests[partition]));
    }
    return 0;
}




static const struct benchmarking_function isend_testall = {
    .state_size = sizeof(struct isend_bench_state),
    .init = &isend_init,
    .start = &isend_start,
    .partition_operation_send = &isend_testall_partition_operation,
    .partition_operation_recv = &isend_testall_partition_operation,
    .complete = &isend_complete,
    .cleanup = &isend_cleanup
};

static const struct benchmarking_function isend_then_test = {
    .state_size = sizeof(struct isend_bench_state),
    .init = &isend_init,
    .start = &isend_start,
    .partition_operation_send = &isend_partition_operation,
    .partition_operation_recv = &isend_partition_operation,
    .complete = &isend_testall_complete,
    .cleanup = &isend_cleanup
};

static const struct benchmarking_function isend_test = {
    .state_size = sizeof(struct isend_bench_state),
    .init = &isend_init,
    .start = &isend_start,
    .partition_operation_send = &isend_test_partition_operation,
    .partition_operation_recv = &isend_test_partition_operation,
    .complete = &isend_complete,
    .cleanup = &isend_cleanup
};

const struct benchmarking_function isend = {
    .state_size = sizeof(struct isend_bench_state),
    .init = &isend_init,
    .start = &isend_start,
    .partition_operation_send = &isend_partition_operation,
    .partition_operation_recv = &isend_partition_operation,
    .complete = &isend_complete,
    .cleanup = &isend_cleanup
};

void bench_isend_testall(TestCase *test_case, Result *result, int comm_rank)
{ 
    execute(test_case, result, comm_rank, isend_testall);
}

void bench_isend_then_test(TestCase *test_case, Result *result, int comm_rank)
{ 
    execute(test_case, result, comm_rank, isend_then_test);
}

void bench_isend_test(TestCase *test_case, Result *result, int comm_rank)
{ 
    execute(test_case, result, comm_rank, isend_test);
}

void bench_isend(TestCase *test_case, Result *result, int comm_rank)
{ 
    execute(test_case, result, comm_rank, isend);
}
