#pragma once


#include "permutation.h"

#include "benchmarks/bench.h"

#include <stdlib.h>



// function pointer type for a transfer step
typedef int (*benchmarking_step)(TestCase *testcase, Result *result, int rank, void* state);

// function pointer type for a partition- and thread-specific transfer step
typedef int (*benchmarking_step_partition)(TestCase *testcase, Result *result, int rank, int partition_num, int thread_num, void* state);

// struct for defining transfer mechanisms
struct benchmarking_function {
    int const state_size;
    benchmarking_step const init;
    benchmarking_step const start;
    benchmarking_step_partition const partition_operation_send;
    benchmarking_step_partition const partition_operation_recv;
    benchmarking_step const complete;
    benchmarking_step const cleanup;
};


//
static void execute(TestCase* testcase, Result* result, int rank, const struct benchmarking_function functions) {
    char state_buf[functions.state_size];     /* allocate state on the stack*/
    void* state = (void*) state_buf;

    if (NULL != functions.init)
        functions.init(testcase, result, rank, state);

    timers timers;
    timers_init(&timers, TimerCount);

    /* warmup */
    for(int it = 0; it < testcase->warmup_iterations; it++) {
        if (NULL != functions.start)
            functions.start(testcase, result, rank, state);

        if (0 == rank) {
            if (NULL != functions.partition_operation_send) {
                #pragma omp parallel for num_threads(testcase->thread_count)
                for (int t = 0; t < testcase->thread_count; t++) {
                    for (int p = 0; p < testcase->partitions_per_thread; p++) {
                        int partition_num = *permutation_at(testcase->send_pattern, p + t * testcase->partitions_per_thread);
                        functions.partition_operation_send(testcase, result, rank, partition_num, t, state);
                    }
                }
            }
        } else {
            if (NULL != functions.partition_operation_recv) {
                #pragma omp parallel for num_threads(testcase->thread_count)
                for (int t = 0; t < testcase->thread_count; t++) {
                    for (int p = 0; p < testcase->partitions_per_thread; p++) {
                        int partition_num = *permutation_at(testcase->send_pattern, p + t * testcase->partitions_per_thread);
                        functions.partition_operation_recv(testcase, result, rank, partition_num, t, state);
                    }
                }
            }
        }

        if (NULL != functions.complete)
            functions.complete(testcase, result, rank, state);
    }

    usleep(testcase->post_warmup_sleep);


    MPI_CHECK(MPI_Barrier(MPI_COMM_WORLD));
    timers_start(timers, Total);

    for (size_t i = 0; i < testcase->iteration_count; i++)
    {
        timers_start(timers, Iteration);
        timers_start(timers, IterationStartToWait);

        if (NULL != functions.start)
            functions.start(testcase, result, rank, state);

        if (0 == rank) {

            if (NULL != functions.partition_operation_send) {
                #pragma omp parallel for num_threads(testcase->thread_count)
                for (int t = 0; t < testcase->thread_count; t++) {
                    for (size_t p = 0; p < testcase->partitions_per_thread; p++) {
                        unsigned int partition_num = *permutation_at(testcase->send_pattern, p + t * testcase->partitions_per_thread);

                        work(testcase->partition_size);

                        functions.partition_operation_send(testcase, result, rank, partition_num, t, state);
                    }
                }
            } else {
                #pragma omp parallel for num_threads(testcase->thread_count)
                for (int t = 0; t < testcase->thread_count; t++) {
                    for (size_t p = 0; p < testcase->partitions_per_thread; p++) {
                        unsigned int partition_num = *permutation_at(testcase->send_pattern, p + t * testcase->partitions_per_thread);

                        work(testcase->partition_size);
                    }
                }
            }
        } else {
            if (NULL != functions.partition_operation_recv) {
                #pragma omp parallel for num_threads(testcase->thread_count)
                for (int t = 0; t < testcase->thread_count; t++) {
                    for (size_t p = 0; p < testcase->partitions_per_thread; p++) {
                        unsigned int partition_num = *permutation_at(testcase->send_pattern, p + t * testcase->partitions_per_thread);

                        work(testcase->partition_size);

                        functions.partition_operation_recv(testcase, result, rank, partition_num, t, state);
                    }
                }
            } else {
                #pragma omp parallel for num_threads(testcase->thread_count)
                for (int t = 0; t < testcase->thread_count; t++) {
                    for (size_t p = 0; p < testcase->partitions_per_thread; p++) {
                        unsigned int partition_num = *permutation_at(testcase->send_pattern, p + t * testcase->partitions_per_thread);

                        work(testcase->partition_size);
                    }
                }
            }
        }

        timers_stop(timers, IterationStartToWait);

        if (NULL != functions.complete)
            functions.complete(testcase, result, rank, state);

        timers_stop(timers, Iteration);
    }

    MPI_CHECK(MPI_Barrier(MPI_COMM_WORLD));
    timers_stop(timers, Total);

    if (NULL != functions.cleanup)
        functions.cleanup(testcase, result, rank, state);

    timers_store(timers, result);
    timers_free(timers);
}


// TODO: actually use offloading
static void execute_gpu(TestCase* testcase, Result* result, int rank, const struct benchmarking_function functions) {
    char state_buf[functions.state_size];     /* allocate state on the stack*/
    void* state = (void*) state_buf;

    if (NULL != functions.init)
        functions.init(testcase, result, rank, state);

    timers timers;
    timers_init(&timers, TimerCount);

    /* warmup */
    for(int it = 0; it < testcase->warmup_iterations; it++) {
        if (NULL != functions.start)
            functions.start(testcase, result, rank, state);

        if (0 == rank) {
            if (NULL != functions.partition_operation_send) {
                #pragma omp parallel for num_threads(testcase->thread_count)
                for (int t = 0; t < testcase->thread_count; t++) {
                    for (int p = 0; p < testcase->partitions_per_thread; p++) {
                        int partition_num = *permutation_at(testcase->send_pattern, p + t * testcase->partitions_per_thread);
                        functions.partition_operation_send(testcase, result, rank, partition_num, t, state);
                    }
                }
            }
        } else {
            if (NULL != functions.partition_operation_recv) {
                #pragma omp parallel for num_threads(testcase->thread_count)
                for (int t = 0; t < testcase->thread_count; t++) {
                    for (int p = 0; p < testcase->partitions_per_thread; p++) {
                        int partition_num = *permutation_at(testcase->send_pattern, p + t * testcase->partitions_per_thread);
                        functions.partition_operation_recv(testcase, result, rank, partition_num, t, state);
                    }
                }
            }
        }

        if (NULL != functions.complete)
            functions.complete(testcase, result, rank, state);
    }
    usleep(testcase->post_warmup_sleep);


    MPI_CHECK(MPI_Barrier(MPI_COMM_WORLD));
    timers_start(timers, Total);

    for (size_t i = 0; i < testcase->iteration_count; i++)
    {
        timers_start(timers, Iteration);
        timers_start(timers, IterationStartToWait);

        if (NULL != functions.start)
            functions.start(testcase, result, rank, state);

        if (0 == rank) {

            if (NULL != functions.partition_operation_send) {
                #pragma omp parallel for num_threads(testcase->thread_count)
                for (int t = 0; t < testcase->thread_count; t++) {
                    for (size_t p = 0; p < testcase->partitions_per_thread; p++) {
                        unsigned int partition_num = *permutation_at(testcase->send_pattern, p + t * testcase->partitions_per_thread);

                        work(testcase->partition_size);

                        functions.partition_operation_send(testcase, result, rank, partition_num, t, state);
                    }
                }
            } else {
                #pragma omp parallel for num_threads(testcase->thread_count)
                for (int t = 0; t < testcase->thread_count; t++) {
                    for (size_t p = 0; p < testcase->partitions_per_thread; p++) {
                        unsigned int partition_num = *permutation_at(testcase->send_pattern, p + t * testcase->partitions_per_thread);

                        work(testcase->partition_size);
                    }
                }
            }
        } else {
            if (NULL != functions.partition_operation_recv) {
                #pragma omp parallel for num_threads(testcase->thread_count)
                for (int t = 0; t < testcase->thread_count; t++) {
                    for (size_t p = 0; p < testcase->partitions_per_thread; p++) {
                        unsigned int partition_num = *permutation_at(testcase->send_pattern, p + t * testcase->partitions_per_thread);

                        work(testcase->partition_size);

                        functions.partition_operation_recv(testcase, result, rank, partition_num, t, state);
                    }
                }
            } else {
                #pragma omp parallel for num_threads(testcase->thread_count)
                for (int t = 0; t < testcase->thread_count; t++) {
                    for (size_t p = 0; p < testcase->partitions_per_thread; p++) {
                        unsigned int partition_num = *permutation_at(testcase->send_pattern, p + t * testcase->partitions_per_thread);

                        work(testcase->partition_size);
                    }
                }
            }
        }

        timers_stop(timers, IterationStartToWait);

        if (NULL != functions.complete)
            functions.complete(testcase, result, rank, state);

        timers_stop(timers, Iteration);
    }

    MPI_CHECK(MPI_Barrier(MPI_COMM_WORLD));
    timers_stop(timers, Total);

    if (NULL != functions.cleanup)
        functions.cleanup(testcase, result, rank, state);

    timers_store(timers, result);
    timers_free(timers);
}


