#include "bench.h"


void bench_psend(TestCase *test_case, Result *result, int comm_rank)
{
    timers timers;
    timers_init(&timers, TimerCount);

    MPI_Request request;

    if (comm_rank == 0)
    {
        MPI_Psend_init(test_case->buffer, test_case->partition_count, test_case->partition_size, MPI_CHAR, 1, 0, MPI_COMM_WORLD, MPI_INFO_ENV, &request);
    } else {
        MPI_Precv_init(test_case->buffer, test_case->partition_count_recv, test_case->partition_size_recv, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_INFO_ENV, &request);
    }

    // warmup
    if (comm_rank == 0)
    {
        MPI_CHECK(MPI_Start(&request));

        #pragma omp parallel for num_threads(test_case->thread_count)
        for (int t = 0; t < test_case->thread_count; t++) {
            for (int p = 0; p < test_case->partitions_per_thread; p++) {
                unsigned int partition_num = *permutation_at(test_case->send_pattern, p + t * test_case->partitions_per_thread);
                MPI_Pready(partition_num, request);
            }
        }

        MPI_CHECK(MPI_Wait(&request, &result->send_status));
    } else if (comm_rank == 1) {
        MPI_CHECK(MPI_Start(&request));
        MPI_CHECK(MPI_Wait(&request, &result->recv_status));
    }
    usleep(POST_WARMUP_SLEEP_US);

    MPI_Barrier(MPI_COMM_WORLD);
    timers_start(timers, Total);

    if (0 == comm_rank) {
        for (int i = 0; i < test_case->iteration_count; i++)
        {
            timers_start(timers, Iteration);
            timers_start(timers, IterationStartToWait);
            MPI_Start(&request);

            #pragma omp parallel for num_threads(test_case->thread_count)
            for (int t = 0; t < test_case->thread_count; t++) {
                for (int p = 0; p < test_case->partitions_per_thread; p++) {
                    unsigned int partition_num = *permutation_at(test_case->send_pattern, p + t * test_case->partitions_per_thread);
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
