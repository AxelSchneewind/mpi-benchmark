#include "benchmarks/bench.h"

#include "benchmarks/bench_psend.h"


#include <stdio.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>

#include <assert.h>

// enable/disables status messages
#define ENABLE_STATUS 0

#if ENABLE_STATUS == 1
#define OUTPUT(x)                         \
    {                                     \
        x;                                \
        printf("%i, R0: " #x "\n", __LINE__); \
        assert(0 == fflush(stdout));      \
    }

#define OUTPUT_RANK(x)                         \
    {                                     \
        x;                                \
        printf("%i, R%i: " #x "\n", __LINE__, comm_rank); \
        assert(0 == fflush(stdout));      \
    }
#else
#define OUTPUT(x) \
    {             \
        x;        \
    }

#define OUTPUT_RANK(x) { x; }

#endif

typedef struct
{
    pthread_t thread;
    volatile MPI_Request *request;
    sem_t request_available;
    sem_t signal_done;
    sem_t accept_requests;
} progress_thread;

#if ENABLE_STATUS == 1
#define progress_thread_check(thread)                                                                      \
    {                                                                                                      \
        int request_available_value;                                                                       \
        int signal_done_value;                                                                             \
        int accept_requests_value;                                                                         \
                                                                                                           \
        sem_getvalue(&thread->request_available, &request_available_value);                                \
        sem_getvalue(&thread->signal_done, &signal_done_value);                                            \
        sem_getvalue(&thread->accept_requests, &accept_requests_value);                                    \
                                                                                                           \
        assert(request_available_value <= 1);                                                              \
        assert(signal_done_value <= 1);                                                                    \
        assert(accept_requests_value <= 1);                                                                \
    }
#else
#define progress_thread_check(thread) ;
#endif

void *run_progress(void *arguments)
{
    progress_thread *thread = (progress_thread *)arguments;
    sem_post(&thread->accept_requests);
    progress_thread_check(thread);

    while (1)
    {
        int ret_trywait = 0;
        int flag;

        OUTPUT(sem_wait(&thread->request_available));
        OUTPUT(sem_wait(&thread->accept_requests));
        if (thread->request == NULL)
            break;
        progress_thread_check(thread);

        ret_trywait = sem_trywait(&thread->signal_done);
        while (ret_trywait != 0) // exit if signal_done is posted
        {
            // currently causes problems on OpenMPI (see https://github.com/open-mpi/ompi/issues/12328)
            MPI_Request_get_status(*thread->request, &flag, MPI_STATUS_IGNORE);
            usleep(100);
            ret_trywait = sem_trywait(&thread->signal_done);
        }

        OUTPUT(sem_post(&thread->accept_requests));
        progress_thread_check(thread);
    }

    return NULL;
}

void progress_thread_create(progress_thread *thread)
{
    thread->request = NULL;
    sem_init(&thread->request_available, 0, 0);
    sem_init(&thread->accept_requests, 0, 0);
    sem_init(&thread->signal_done, 0, 0);

    pthread_create(&thread->thread, 0, run_progress, thread);
    progress_thread_check(thread);
}

void progress_thread_set_request(progress_thread *thread, MPI_Request *request)
{
    progress_thread_check(thread);
    sem_wait(&thread->accept_requests);
    thread->request = request;
    sem_post(&thread->accept_requests);
    progress_thread_check(thread);
}

void progress_thread_pause(progress_thread *thread)
{
    progress_thread_check(thread);
    sem_post(&thread->signal_done);

    sem_wait(&thread->accept_requests);
    sem_post(&thread->accept_requests);

    progress_thread_check(thread);
}

void progress_thread_continue(progress_thread *thread)
{
    progress_thread_check(thread);
    sem_trywait(&thread->request_available);
    sem_post(&thread->request_available);
    progress_thread_check(thread);
}

void progress_thread_destroy(progress_thread *thread)
{
    progress_thread_check(thread);
    progress_thread_set_request(thread, NULL);
    progress_thread_continue(thread);

    pthread_join(thread->thread, NULL);

    sem_destroy(&thread->signal_done);
    sem_destroy(&thread->accept_requests);
    sem_destroy(&thread->request_available);
}



struct psend_progress_thread_bench_state {
    MPI_Request request;
    MPI_Status status;
    progress_thread progress;
};


static int psend_progress_thread_init(TestCase *test_case, Result *result, int comm_rank, void* s) {
    struct psend_progress_thread_bench_state* state = (struct psend_progress_thread_bench_state*) s;
    if (0 == comm_rank) {
        MPI_CHECK(MPI_Psend_init(test_case->buffer, test_case->buffer_size / test_case->partition_size, test_case->partition_size, MPI_CHAR, 1, 0, MPI_COMM_WORLD, MPI_INFO_ENV, &state->request));
    } else {
        MPI_CHECK(MPI_Precv_init(test_case->buffer, test_case->buffer_size / test_case->partition_size_recv, test_case->partition_size_recv, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_INFO_ENV, &state->request));
    }

    progress_thread_create(&state->progress);

    return 0;
}

static int psend_progress_thread_cleanup(TestCase *test_case, Result *result, int comm_rank, void* s) {
    struct psend_progress_thread_bench_state* state = (struct psend_progress_thread_bench_state*) s;
    progress_thread_destroy(&state->progress);
    MPI_CHECK(MPI_Request_free(&state->request));
    return 0;
}

static int psend_progress_thread_start(TestCase *test_case, Result *result, int comm_rank, void* s) {
    struct psend_progress_thread_bench_state* state = (struct psend_progress_thread_bench_state*) s;
    MPI_CHECK(MPI_Start(&state->request));

    if (0 == comm_rank) {
        progress_thread_set_request(&state->progress, &state->request);
        progress_thread_continue(&state->progress);
    }

    return 0;
}


static int psend_progress_thread_complete(TestCase *test_case, Result *result, int comm_rank, void* s) {
    struct psend_progress_thread_bench_state* state = (struct psend_progress_thread_bench_state*) s;

    if (0 == comm_rank) {
        progress_thread_pause(&state->progress);
    }

    MPI_CHECK(MPI_Wait(&state->request, MPI_STATUSES_IGNORE));
    return 0;
}


static const struct benchmarking_function psend_progress_thread = {
    .state_size = sizeof(struct psend_progress_thread_bench_state),
    .init = &psend_progress_thread_init,
    .start = &psend_progress_thread_start,
    .partition_operation_send = &psend_send_partition_operation,
    .partition_operation_recv = &psend_recv_partition_operation,
    .complete = &psend_progress_thread_complete,
    .cleanup = &psend_progress_thread_cleanup
};

void bench_psend_progress_thread(TestCase *test_case, Result *result, int comm_rank)
{ 
    execute(test_case, result, comm_rank, psend_progress_thread);
}
