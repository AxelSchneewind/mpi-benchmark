#include "bench.h"
#include "stdio.h"


void bench_isend(TestCase *test_case, Result *result, int comm_rank)
{
    MPI_Request *requests = malloc(sizeof(MPI_Request) * test_case->partition_count);

    timers timers;
    timers_init(&timers, TimerCount);

    // warmup
    if (comm_rank == 0) {
        #pragma omp parallel for num_threads(test_case->thread_count)
        for (int t = 0; t < test_case->thread_count; t++) {
            for (size_t p = 0; p < test_case->partitions_per_thread; p++) {
                MPI_CHECK(MPI_Isend(test_case->buffer, test_case->partition_size, MPI_BYTE, 1, p, MPI_COMM_WORLD, &requests[p]));
                MPI_CHECK(MPI_Wait(&requests[p], MPI_STATUS_IGNORE));
            }
        }
    } else {
        #pragma omp parallel for num_threads(test_case->thread_count)
        for (int t = 0; t < test_case->thread_count; t++) {
            for (size_t p = 0; p < test_case->partitions_per_thread; p++) {
                MPI_CHECK(MPI_Irecv(test_case->buffer, test_case->partition_size, MPI_BYTE, 0, p, MPI_COMM_WORLD, &requests[p]));
                MPI_CHECK(MPI_Wait(&requests[p], MPI_STATUS_IGNORE));
            }
        }
    }

    MPI_CHECK(MPI_Barrier(MPI_COMM_WORLD));
    timers_start(timers, Total);

    for (size_t i = 0; i < test_case->iteration_count; i++)
    {
        if (comm_rank == 0)
        {
            timers_start(timers, Iteration);
            timers_start(timers, IterationStartToWait);

            #pragma omp parallel for num_threads(test_case->thread_count)
            for (int t = 0; t < test_case->thread_count; t++) {
                for (size_t p = 0; p < test_case->partitions_per_thread; p++)
                {
                    unsigned int partition_num = test_case->send_pattern[p + t * test_case->partitions_per_thread];				
                    work(test_case->partition_size);
                    MPI_CHECK(MPI_Isend(test_case->buffer + partition_num * test_case->partition_size, test_case->partition_size, MPI_BYTE, 1, partition_num, MPI_COMM_WORLD, &requests[partition_num]));
                }
            }

            timers_stop(timers, IterationStartToWait);
            MPI_CHECK(MPI_Waitall(test_case->partition_count, requests, MPI_STATUSES_IGNORE));

            timers_stop(timers, Iteration);
        }
        else if (comm_rank == 1)
        {
            timers_start(timers, Iteration);
            timers_start(timers, IterationStartToWait);

            #pragma omp parallel for num_threads(test_case->thread_count)
            for (int t = 0; t < test_case->thread_count; t++) {
                for (size_t p = 0; p < test_case->partitions_per_thread; p++)
                {
                    unsigned int partition_num = test_case->recv_pattern[p + t * test_case->partitions_per_thread];
                    MPI_CHECK(MPI_Irecv(test_case->buffer + partition_num * test_case->partition_size, test_case->partition_size, MPI_BYTE, 0, partition_num, MPI_COMM_WORLD, &requests[partition_num]));
                }
            }

            timers_stop(timers, IterationStartToWait);
            MPI_CHECK(MPI_Waitall(test_case->partition_count, requests, MPI_STATUSES_IGNORE));

            timers_stop(timers, Iteration);
        }
    }

    MPI_CHECK(MPI_Barrier(MPI_COMM_WORLD));
    timers_stop(timers, Total);

    timers_store(timers, result);
    timers_free(timers);

    free(requests);
};