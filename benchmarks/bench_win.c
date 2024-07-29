#include "benchmarks/bench.h"

void bench_win(TestCase *test_case, Result *result, int comm_rank)
{
    MPI_CHECK(MPI_Barrier(MPI_COMM_WORLD));

    // init windows
    MPI_Win *windows = malloc(sizeof(MPI_Win) * test_case->partition_count);
    for (size_t p = 0; p < test_case->partition_count; p++)
    {
        MPI_CHECK(MPI_Win_create(test_case->buffer + p * test_case->partition_size, test_case->partition_size, 1, MPI_INFO_ENV, MPI_COMM_WORLD, &windows[p]));
    }

    timers timers;
    timers_init(&timers, TimerCount);

    // warmup
    for(int it = 0; it < WARMUP_ITERATIONS; it++) {
        if (0 == comm_rank){
            for (size_t p = 0; p < test_case->partition_count; p++)
            {
                unsigned int partition_num = p;
                MPI_CHECK(MPI_Win_lock(MPI_LOCK_EXCLUSIVE, 1, 0, windows[partition_num]));
                MPI_CHECK(MPI_Put(
                    test_case->buffer + test_case->partition_size * partition_num,
                    test_case->partition_size, MPI_BYTE, 1, 
                    0, test_case->partition_size, MPI_BYTE, windows[partition_num]));
                MPI_CHECK(MPI_Win_unlock(1, windows[partition_num]));
            }
        } else {
            for (size_t p = 0; p < test_case->partition_count; p++)
            {
                MPI_CHECK(MPI_Win_lock(MPI_LOCK_EXCLUSIVE, 1, 0, windows[p]));
                MPI_CHECK(MPI_Win_unlock(1, windows[p]));
            }
        }
    }
    usleep(POST_WARMUP_SLEEP_US);

    MPI_CHECK(MPI_Barrier(MPI_COMM_WORLD));
    timers_start(timers, Total);

    for (size_t i = 0; i < test_case->iteration_count; i++)
    {
        if (comm_rank == 0)
        {
            timers_start(timers, Iteration);
            timers_start(timers, IterationStartToWait);

            for (size_t p = 0; p < test_case->partition_count; p++)
            {
                unsigned int partition_num = *permutation_at(test_case->send_pattern, p);
                work(test_case->partition_size);
                MPI_CHECK(MPI_Win_lock(MPI_LOCK_EXCLUSIVE, 1, MPI_MODE_NOCHECK, windows[partition_num]));
                MPI_CHECK(MPI_Put(
                    test_case->buffer + test_case->partition_size * partition_num,
                    test_case->partition_size, MPI_BYTE, 1, 0,
                    test_case->partition_size, MPI_BYTE, windows[partition_num]));
                MPI_CHECK(MPI_Win_unlock(1, windows[partition_num]));
            }

            timers_stop(timers, IterationStartToWait);
            timers_stop(timers, Iteration);
        }
    }

    MPI_CHECK(MPI_Barrier(MPI_COMM_WORLD));
    timers_stop(timers, Total);

    for (size_t partition = 0; partition < test_case->partition_count; partition++)
    {
        MPI_CHECK(MPI_Win_free(&windows[partition]));
    }
    free(windows);

    timers_store(timers, result);
    timers_free(timers);
};