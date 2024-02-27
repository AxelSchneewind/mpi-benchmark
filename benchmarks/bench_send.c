#include "bench.h"
#include "stdio.h"

void bench_send(TestCase *test_case, Result *result, int comm_rank)
{
	timers timers;
	timers_init(&timers, TimerCount);

	// warmup
	if (comm_rank == 0) {
		MPI_CHECK(MPI_Send(test_case->buffer, test_case->partition_size, MPI_BYTE, 1, 0, MPI_COMM_WORLD));
	} else {
		MPI_CHECK(MPI_Recv(test_case->buffer, test_case->partition_size, MPI_BYTE, 0, 0, MPI_COMM_WORLD, &result->recv_status));
	}

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
				unsigned int partition_num = test_case->send_pattern[p];
				work(test_case->partition_size);
				MPI_CHECK(MPI_Send(test_case->buffer + test_case->partition_size * partition_num, test_case->partition_size, MPI_BYTE, 1, 0, MPI_COMM_WORLD));
			}

			timers_stop(timers, IterationStartToWait);
			timers_stop(timers, Iteration);
		}
		else if (comm_rank == 1)
		{
			timers_start(timers, Iteration);
			timers_start(timers, IterationStartToWait);

			for (size_t p = 0; p < test_case->partition_count; p++)
			{
				unsigned int partition_num = test_case->send_pattern[p];
				MPI_CHECK(MPI_Recv(test_case->buffer + test_case->partition_size * partition_num, test_case->partition_size, MPI_BYTE, 0, 0, MPI_COMM_WORLD, &result->recv_status));
			}

			timers_stop(timers, IterationStartToWait);
			timers_stop(timers, Iteration);
		}
	}

	MPI_CHECK(MPI_Barrier(MPI_COMM_WORLD));
	timers_stop(timers, Total);

	timers_store(timers, result);
	timers_free(timers);
};