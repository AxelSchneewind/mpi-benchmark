#include "bench.h"
#include "stdio.h"

#define LOG(format)                                               \
	{                                                             \
		printf("(R%i) [%s:%i]: ", comm_rank, __FILE__, __LINE__); \
		printf(format);                                           \
		printf("\n");                                             \
	}
#define LOGF(format, args...)                                     \
	{                                                             \
		printf("(R%i) [%s:%i]: ", comm_rank, __FILE__, __LINE__); \
		printf(format, args);                                     \
		printf("\n");                                             \
	}

void bench_win(TestCase *test_case, Result *result, int comm_rank)
{
	MPI_Barrier(MPI_COMM_WORLD);

	// init windows
	MPI_Win *windows = malloc(sizeof(MPI_Win) * test_case->partition_count);
	for (size_t p = 0; p < test_case->partition_count; p++)
	{
		MPI_Win_create(test_case->buffer + p * test_case->partition_size, test_case->partition_size, 1, MPI_INFO_ENV, MPI_COMM_WORLD, &windows[p]);
	}

	timer *timers;
	timers_init(&timers, test_case, result);

	MPI_Barrier(MPI_COMM_WORLD);
	timers_start_global(timers);

	for (size_t i = 0; i < test_case->iteration_count; i++)
	{
		if (comm_rank == 0)
		{
			timers_start_local(timers);

			for (size_t p = 0; p < test_case->partition_count; p++)
			{
				unsigned int partition_num = test_case->send_pattern[p];
				work(test_case->partition_size);
				MPI_Win_lock(MPI_LOCK_EXCLUSIVE, 1, MPI_MODE_NOCHECK, windows[partition_num]);
				MPI_Put(
					test_case->buffer + test_case->partition_size * partition_num,
					test_case->partition_size, MPI_BYTE, 1,
					0,
					test_case->partition_size, MPI_BYTE, windows[partition_num]);
				MPI_Win_unlock(1, windows[partition_num]);
			}

			timers_stop_local(timers);
		}
	}

	MPI_Barrier(MPI_COMM_WORLD);
	timers_stop_global(timers);

	for (size_t partition = 0; partition < test_case->partition_count; partition++)
	{
		MPI_Win_free(&windows[partition]);
	}
	MPI_Barrier(MPI_COMM_WORLD);

	// clear
	free(windows);

	timers_store(timers, result);
	timers_free(timers);
};