#include "bench.h"


void bench_psend(TestCase *test_case, Result *result, int comm_rank, int comm_ranks)
{
    timers timers;
    timers_init(&timers, TimerCount);

    int recv_from = (comm_rank + comm_ranks - 1) % comm_ranks;
    int send_to   = (comm_rank + comm_ranks + 1) % comm_ranks;

    MPI_Request send_request, recv_request;

    if (comm_rank % 2 == 0) {
        MPI_Psend_init(test_case->buffer, test_case->partition_count, test_case->partition_size, MPI_CHAR, send_to, 0, MPI_COMM_WORLD, MPI_INFO_ENV, &send_request);
        MPI_Precv_init(test_case->buffer, test_case->partition_count_recv, test_case->partition_size_recv, MPI_CHAR, recv_from, 0, MPI_COMM_WORLD, MPI_INFO_ENV, &recv_request);
    } else {
        MPI_Precv_init(test_case->buffer, test_case->partition_count_recv, test_case->partition_size_recv, MPI_CHAR, recv_from, 0, MPI_COMM_WORLD, MPI_INFO_ENV, &recv_request);
        MPI_Psend_init(test_case->buffer, test_case->partition_count, test_case->partition_size, MPI_CHAR, send_to, 0, MPI_COMM_WORLD, MPI_INFO_ENV, &send_request);
    }

    // warmup
    if (comm_rank == 0)
    {
        MPI_CHECK(MPI_Start(&send_request));

        #pragma omp parallel for num_threads(test_case->thread_count)
        for (int t = 0; t < test_case->thread_count; t++) {
            for (int p = 0; p < test_case->partitions_per_thread; p++) {
                unsigned int index = p + t * test_case->partitions_per_thread;
                if (index >= test_case->partition_count) continue;
                unsigned int partition_num = test_case->send_pattern[index];
                MPI_Pready(partition_num, send_request);
            }
        }
        MPI_CHECK(MPI_Wait(&send_request, &result->send_status));

        MPI_CHECK(MPI_Start(&recv_request));
        MPI_CHECK(MPI_Wait(&recv_request, &result->recv_status));
    } else if (comm_rank == 1) {
        MPI_CHECK(MPI_Start(&recv_request));
        MPI_CHECK(MPI_Wait(&recv_request, &result->recv_status));

        MPI_CHECK(MPI_Start(&send_request));
        #pragma omp parallel for num_threads(test_case->thread_count)
        for (int t = 0; t < test_case->thread_count; t++) {
            for (int p = 0; p < test_case->partitions_per_thread; p++) {
                unsigned int index = p + t * test_case->partitions_per_thread;
                if (index >= test_case->partition_count) continue;
                unsigned int partition_num = test_case->send_pattern[index];
                MPI_Pready(partition_num, send_request);
            }
        }
        MPI_CHECK(MPI_Wait(&send_request, &result->send_status));

    }

    MPI_Barrier(MPI_COMM_WORLD);
    timers_start(timers, Total);

    if (0 == comm_rank) {
        for (int i = 0; i < test_case->iteration_count; i++)
        {
            // send
            timers_start(timers, Iteration);
            timers_start(timers, IterationStartToWait);
            MPI_Start(&send_request);

            #pragma omp parallel for num_threads(test_case->thread_count)
            for (int t = 0; t < test_case->thread_count; t++) {
                for (int p = 0; p < test_case->partitions_per_thread; p++) {
                    unsigned int index = p + t * test_case->partitions_per_thread;
                    if (index >= test_case->partition_count) continue;
                    unsigned int partition_num = test_case->send_pattern[index];
                    work(test_case->partition_size);
                    MPI_Pready(partition_num, send_request);
                }
            }

            timers_stop(timers, IterationStartToWait);
            MPI_Wait(&send_request, MPI_STATUS_IGNORE);
            timers_stop(timers, Iteration);

            // recv
            MPI_Start(&recv_request);
            MPI_Wait(&recv_request, MPI_STATUS_IGNORE);
        }
    } else {
        for (int i = 0; i < test_case->iteration_count; i++)
        {
            // recv
            timers_start(timers, Iteration);
            timers_start(timers, IterationStartToWait);
            MPI_Start(&recv_request);

            timers_stop(timers, IterationStartToWait);
            MPI_Wait(&recv_request, MPI_STATUS_IGNORE);
            timers_stop(timers, Iteration);

            // send
            MPI_Start(&send_request);

            #pragma omp parallel for num_threads(test_case->thread_count)
            for (int t = 0; t < test_case->thread_count; t++) {
                for (int p = 0; p < test_case->partitions_per_thread; p++) {
                    unsigned int index = p + t * test_case->partitions_per_thread;
                    if (index >= test_case->partition_count) continue;
                    unsigned int partition_num = test_case->send_pattern[index];
                    work(test_case->partition_size);
                    MPI_Pready(partition_num, send_request);
                }
            }

            MPI_Wait(&send_request, MPI_STATUS_IGNORE);
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Request_free(&send_request);
    MPI_Request_free(&recv_request);
    timers_stop(timers, Total);

    timers_store(timers, result);
    timers_free(timers);
};
