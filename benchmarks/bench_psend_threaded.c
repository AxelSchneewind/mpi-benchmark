#include "bench.h"

void bench_psend_threaded(TestCase *test_case, Result *result, int comm_rank)
{
    timer *timers;
    timers_init(&timers);

    MPI_Request request;

    int thread_count = 8;
    if (test_case->partition_count < thread_count)
        thread_count = test_case->partition_count;

    if (comm_rank == 0)
    {
        MPI_Psend_init(test_case->buffer, test_case->partition_count, test_case->partition_size, MPI_CHAR, 1, 0, MPI_COMM_WORLD, MPI_INFO_ENV, &request);
    } else {
        MPI_Precv_init(test_case->buffer, test_case->partition_count_recv, test_case->partition_size_recv, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_INFO_ENV, &request);
    }

    MPI_Barrier(MPI_COMM_WORLD);
    timers_start(timers, Total);

    if (0 == comm_rank) {
        for (int i = 0; i < test_case->iteration_count; i++)
        {
            timers_start(timers, Iteration);
            timers_start(timers, IterationStartToWait);
            MPI_Start(&request);

            int partitions_per_thread = test_case->partition_count / thread_count;
            #pragma omp parallel for
            for (int i = 0; i < thread_count; i++) {
                for (int p = 0; p < partitions_per_thread; p++)
                {
                    unsigned int partition_num = test_case->send_pattern[p + i * partitions_per_thread];
                    work(test_case->partition_size);
                    MPI_Pready(partition_num, request);
                }
            }

            timers_stop(timers, IterationStartToWait);
            MPI_Wait(&request, MPI_STATUS_IGNORE);
            timers_stop(timers, Iteration);
        }
    } else {
        for (int i = 0; i < test_case->iteration_count; i++)
        {
            timers_start(timers, Iteration);
            timers_start(timers, IterationStartToWait);
            MPI_Start(&request);

            timers_stop(timers, IterationStartToWait);
            MPI_Wait(&request, MPI_STATUS_IGNORE);
            timers_stop(timers, Iteration);
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Request_free(&request);
    timers_stop(timers, Total);

    timers_store(timers, result);
    timers_free(timers);
};
