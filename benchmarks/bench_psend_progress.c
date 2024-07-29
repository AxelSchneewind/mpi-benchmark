#include "benchmarks/bench.h"

#include <stdbool.h>
#include <assert.h>
#include <stdio.h>

void bench_psend_progress(TestCase *test_case, Result *result, int comm_rank)
{
    // init request and timer
    MPI_Request request;

    timers timers;
    timers_init(&timers, TimerCount);

    if (comm_rank == 0)
    {
        MPI_CHECK(MPI_Psend_init(test_case->buffer, test_case->partition_count, test_case->partition_size, MPI_CHAR, 1, 0, MPI_COMM_WORLD, MPI_INFO_ENV, &request));
    }
    else if (comm_rank == 1)
    {
        MPI_CHECK(MPI_Precv_init(test_case->buffer, test_case->partition_count_recv, test_case->partition_size_recv, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_INFO_ENV, &request));
    }

    // warmup
    for(int it = 0; it < WARMUP_ITERATIONS; it++) {
        if (comm_rank == 0)
        {
            MPI_CHECK(MPI_Start(&request));

            for (size_t p = 0; p < test_case->partition_count; p++)
            {
                unsigned int partition_num = p;
                MPI_CHECK(MPI_Pready(partition_num, request));
            }

            MPI_CHECK(MPI_Wait(&request, &result->send_status));
        } else if (comm_rank == 1) {
            MPI_CHECK(MPI_Start(&request));
            MPI_CHECK(MPI_Wait(&request, &result->recv_status));
        }
    }
    usleep(POST_WARMUP_SLEEP_US);

    // run
    MPI_CHECK(MPI_Barrier(MPI_COMM_WORLD));
    timers_start(timers, Total);

    if (comm_rank == 0)
    {
        for (size_t i = 0; i < test_case->iteration_count; i++)
        {
            timers_start(timers, Iteration);
            timers_start(timers, IterationStartToWait);

            // printf("s1\n"); fflush(stdout);
            MPI_CHECK(MPI_Start(&request));

            #pragma omp parallel for num_threads(test_case->thread_count)
            for (int t = 0; t < test_case->thread_count; t++) {
                int flag = false;
                for (int p = 0; p < test_case->partitions_per_thread; p++) {
                    unsigned int partition_num = *permutation_at(test_case->send_pattern, p + t * test_case->partitions_per_thread);

                    // try to trigger progress as MPI_Pready doesn't (at least for openmpi)
                    // currently causes problems on OpenMPI (see https://github.com/open-mpi/ompi/issues/12328)
                    MPI_CHECK(MPI_Request_get_status(request, &flag, &result->send_status));

                    work(test_case->partition_size);

                    /* 
                     * MPI_Request_get_status sometimes returns true despite not all Pready calls being done
                     */
                    //if (flag) {            
                    //     printf("MPI_Request_get_status() returned true before MPI_Pready done (partition %i of %i, iteration %i)\n", p, test_case->partition_count, i); 
                    //     fflush(stdout);
                        // break;
                    //}
                    MPI_CHECK(MPI_Pready(partition_num, request));
                }
            }

            // also, sometimes gets stuck here, but only if MPI_Request_get_status is called
            timers_stop(timers, IterationStartToWait);
            MPI_CHECK(MPI_Wait(&request, &result->send_status));

            timers_stop(timers, Iteration);
        }
    } else if (comm_rank == 1) {
        for (size_t i = 0; i < test_case->iteration_count; i++)
        {
            timers_start(timers, Iteration);
            timers_start(timers, IterationStartToWait);

            MPI_CHECK(MPI_Start(&request));

            // int flag = false;
            // while(!flag)
            // {
            //     // currently causes problems on OpenMPI (see https://github.com/open-mpi/ompi/issues/12328)
            //     MPI_CHECK(MPI_Request_get_status(request, &flag, MPI_STATUS_IGNORE));
            // }

            timers_stop(timers, IterationStartToWait);
            MPI_CHECK(MPI_Wait(&request, MPI_STATUS_IGNORE));

            timers_stop(timers, Iteration);
        }
    }

    MPI_CHECK(MPI_Barrier(MPI_COMM_WORLD));
    timers_stop(timers, Total);

    MPI_CHECK(MPI_Request_free(&request));

    timers_store(timers, result);
    timers_free(timers);
};
