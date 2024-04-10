#include "bench.h"
#include "stdio.h"


void bench_send_persistent(TestCase *test_case, Result *result, int comm_rank)
{
    timers timers;
    timers_init(&timers, TimerCount);

    MPI_Request* requests = calloc(sizeof(MPI_Request), test_case->partition_count);

    // initialize send and receive
    if (0 == comm_rank) {
        for (int i = 0; i < test_case->partition_count; i++) {
            MPI_Send_init(test_case->buffer + test_case->partition_size * i, test_case->partition_size, MPI_CHAR, 1, i, MPI_COMM_WORLD, &requests[i]);
        }
    } else {
        for (int i = 0; i < test_case->partition_count; i++) {
            MPI_Recv_init(test_case->buffer + test_case->partition_size * i, test_case->partition_size, MPI_CHAR, 0, i, MPI_COMM_WORLD, &requests[i]);
        }
    }

    MPI_CHECK(MPI_Barrier(MPI_COMM_WORLD));
    timers_start(timers, Total);

    for (size_t i = 0; i < test_case->iteration_count; i++)
    {
        if (comm_rank == 0)
        {
            MPI_Barrier(MPI_COMM_WORLD);
            timers_start(timers, Iteration);
            timers_start(timers, IterationStartToWait);

            #pragma omp parallel for num_threads(test_case->thread_count)
            for (int t = 0; t < test_case->thread_count; t++) {
                for (int p = 0; p < test_case->partitions_per_thread; p++) {
                    unsigned int partition_num = test_case->send_pattern[p + t * test_case->partitions_per_thread];
                    work(test_case->partition_size);
                    MPI_CHECK(MPI_Start(&requests[partition_num]));
                }
            }

            timers_stop(timers, IterationStartToWait);
            MPI_CHECK(MPI_Waitall(test_case->partition_count, requests, MPI_STATUSES_IGNORE));
            timers_stop(timers, Iteration);
        }
        else if (comm_rank == 1)
        {
            MPI_Barrier(MPI_COMM_WORLD);
            timers_start(timers, Iteration);
            timers_start(timers, IterationStartToWait);

            #pragma omp parallel for num_threads(test_case->thread_count)
            for (int t = 0; t < test_case->thread_count; t++) {
                for (int p = 0; p < test_case->partitions_per_thread; p++) {
                    unsigned int partition_num = test_case->recv_pattern[p + t * test_case->partitions_per_thread];
                    MPI_CHECK(MPI_Start(&requests[partition_num]));
                }
            }

            timers_stop(timers, IterationStartToWait);
            MPI_CHECK(MPI_Waitall(test_case->partition_count, requests, MPI_STATUSES_IGNORE));
            timers_stop(timers, Iteration);
        }
    }

    MPI_CHECK(MPI_Barrier(MPI_COMM_WORLD));
    timers_stop(timers, Total);

    for (int i = 0; i < test_case->partition_count; i++) {
        MPI_Request_free(&requests[i]);
    }
    free(requests);

    timers_store(timers, result);
    timers_free(timers);
};