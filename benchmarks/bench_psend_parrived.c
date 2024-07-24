#include "bench.h"


void bench_psend_parrived(TestCase *test_case, Result *result, int comm_rank)
{
    // init requests and timer
    MPI_Request request;

    timers timers;
    timers_init(&timers, TimerCount);

    // init
    if (comm_rank == 0)
    {
        MPI_CHECK(MPI_Psend_init(test_case->buffer, test_case->partition_count, test_case->partition_size, MPI_CHAR, 1, 0, MPI_COMM_WORLD, MPI_INFO_ENV, &request));
    }
    else if (comm_rank == 1)
    {
        MPI_CHECK(MPI_Precv_init(test_case->buffer, test_case->partition_count_recv, test_case->partition_size_recv, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_INFO_ENV, &request));
    }

    // warmup
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

            MPI_CHECK(MPI_Start(&request));

            #pragma omp parallel for num_threads(test_case->thread_count)
            for (int t = 0; t < test_case->thread_count; t++) {
                for (int p = 0; p < test_case->partitions_per_thread; p++) {
                    unsigned int partition_num = *permutation_at(test_case->send_pattern, p + t * test_case->partitions_per_thread);
                    work(test_case->partition_size);

                    // 
                    MPI_CHECK(MPI_Pready(partition_num, request));
                }
            }

            timers_stop(timers, IterationStartToWait);
            MPI_CHECK(MPI_Wait(&request, &result->send_status));

            timers_stop(timers, Iteration);
        }
    }
    else if (comm_rank == 1)
    {
        for (size_t i = 0; i < test_case->iteration_count; i++)
        {
			
            timers_start(timers, Iteration);
            timers_start(timers, IterationStartToWait);

            MPI_CHECK(MPI_Start(&request));

            int flag = 0;
            while (!flag)
            {
                // TODO fix by computing partitions_per_threads for receive side too
                // #pragma omp parallel for num_threads(test_case->thread_count)
                // for (int t = 0; t < test_case->thread_count; t++) {
                    int _flag = 0;
                    for (int p = 0; p < /*test_case->partitions_per_thread*/ test_case->partition_count_recv; p++) {
                        MPI_CHECK(MPI_Request_get_status(request, &_flag, &result->recv_status));
                        if (_flag) {		// enable if something goes wrong 
                            flag = _flag;
                        }

                        if (flag) {
                            break;
                        }

                        // ignoring result of MPI_Parrived
                        MPI_CHECK(MPI_Parrived(request, p /*+ t * test_case->partitions_per_thread*/, &_flag));
                    }
                // }
            }

            timers_stop(timers, IterationStartToWait);
            MPI_CHECK(MPI_Wait(&request, &result->recv_status));

            timers_stop(timers, Iteration);
        }
    }

    MPI_CHECK(MPI_Barrier(MPI_COMM_WORLD));
    timers_stop(timers, Total);

    MPI_CHECK(MPI_Request_free(&request));

    timers_store(timers, result);
    timers_free(timers);
};
