#include "benchmarks/bench_template.h"

#include <stdio.h>
#include <assert.h>

struct send_bench_state {
    MPI_Request* requests;
    MPI_Status* statuses;
};


static int send_init(TestCase *test_case, Result *result, int comm_rank, void* s) {
    struct send_bench_state* state = (struct send_bench_state*) s;

    state->requests = calloc(sizeof(MPI_Request), test_case->partition_count);
    state->statuses = calloc(sizeof(MPI_Status), test_case->partition_count);

    if (0 == comm_rank) {
        for (int i = 0; i < test_case->partition_count; i++) {
            MPI_CHECK(MPI_Send_init(test_case->buffer + test_case->partition_size * i, test_case->partition_size, MPI_CHAR, 1, i, MPI_COMM_WORLD, &state->requests[i]));
        }
    } else {
        for (int i = 0; i < test_case->partition_count; i++) {
            MPI_CHECK(MPI_Recv_init(test_case->buffer + test_case->partition_size * i, test_case->partition_size, MPI_CHAR, 0, i, MPI_COMM_WORLD, &state->requests[i]));
        }
    }
    return 0;
}

static int send_cleanup(TestCase *test_case, Result *result, int comm_rank, void* s) {
    struct send_bench_state* state = (struct send_bench_state*) s;
    for (int i = 0; i < test_case->partition_count; i++) {
        MPI_CHECK(MPI_Request_free(&state->requests[i]));
    }
    free(state->requests);
    free(state->statuses);
    state->requests = NULL;
    state->statuses = NULL;
    return 0;
}

static int send_start(TestCase *test_case, Result *result, int comm_rank, void* s) {
    return 0;
}


static int send_complete(TestCase *test_case, Result *result, int comm_rank, void* s) {
    struct send_bench_state* state = (struct send_bench_state*) s;
    MPI_CHECK(MPI_Waitall(test_case->partition_count, state->requests, MPI_STATUSES_IGNORE));
    return 0;
}

static int send_recv_partition_operation(TestCase *test_case, Result *result, int comm_rank, int partition, int threadnum, void* s) {
    struct send_bench_state* state = (struct send_bench_state*) s;
    MPI_CHECK(MPI_Start(&state->requests[partition]));
    return 0;
}

static int send_send_partition_operation(TestCase *test_case, Result *result, int comm_rank, int partition, int threadnum, void* s) {
    struct send_bench_state* state = (struct send_bench_state*) s;
    assert (0 == comm_rank);
    MPI_CHECK(MPI_Start(&state->requests[partition]));
    return 0;
}

static int send_test_send_partition_operation(TestCase *test_case, Result *result, int comm_rank, int partition, int threadnum, void* s) {
    struct send_bench_state* state = (struct send_bench_state*) s;
    assert (0 == comm_rank);
    int flag;
    MPI_CHECK(MPI_Start(&state->requests[partition]));
    MPI_CHECK(MPI_Request_get_status(state->requests[partition], &flag, &state->statuses[partition]));
    return 0;
}


static const struct benchmarking_function send_persistent_test = {
    .state_size = sizeof(struct send_bench_state),
    .init = &send_init,
    .start = &send_start,
    .partition_operation_send = &send_test_send_partition_operation,
    .partition_operation_recv = &send_recv_partition_operation,
    .complete = &send_complete,
    .cleanup = &send_cleanup
};

static const struct benchmarking_function send_persistent = {
    .state_size = sizeof(struct send_bench_state),
    .init = &send_init,
    .start = &send_start,
    .partition_operation_send = &send_send_partition_operation,
    .partition_operation_recv = &send_recv_partition_operation,
    .complete = &send_complete,
    .cleanup = &send_cleanup
};

void bench_send_persistent_test(TestCase *test_case, Result *result, int comm_rank)
{ 
    execute(test_case, result, comm_rank, send_persistent_test);
}

void bench_send_persistent(TestCase *test_case, Result *result, int comm_rank)
{ 
    execute(test_case, result, comm_rank, send_persistent);
}
