#include "bench.h"
#include "stdio.h"

void bench_psend(TestCase *test_case, Result *result, int comm_rank)
{
	// init
	MPI_Request request;

	timer* timers;
	timers_init(&timers, test_case, result);

	if (comm_rank == 0)
	{
		MPI_Psend_init(test_case->buffer, test_case->partition_count, test_case->partition_size, MPI_CHAR, 1, 0, MPI_COMM_WORLD, MPI_INFO_ENV, &request);
	}
	else if (comm_rank == 1)
	{
		MPI_Precv_init(test_case->buffer, test_case->partition_count_recv, test_case->partition_size_recv, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_INFO_ENV, &request);
	}

	// run
	MPI_Barrier(MPI_COMM_WORLD);
	timers_start_global(timers);

	for (size_t i = 0; i < test_case->iteration_count; i++)
	{
		if (comm_rank == 0)
		{
			timers_start_local(timers);

			MPI_Start(&request);

			for (size_t p = 0; p < test_case->partition_count; p++) {
				unsigned int partition_num = test_case->send_pattern[p];
				work(test_case->partition_size);
				MPI_Pready(partition_num, request);
			}

			MPI_Wait(&request, &result->send_status);

			timers_stop_local(timers);
		}
		else if (comm_rank == 1)
		{
			timers_start_local(timers);

			MPI_Start(&request);
			MPI_Wait(&request, &result->recv_status);

			timers_stop_local(timers);
		}
	}

	MPI_Barrier(MPI_COMM_WORLD);
	timers_stop_global(timers);

	timers_store(timers, result);
	timers_free(timers);
};