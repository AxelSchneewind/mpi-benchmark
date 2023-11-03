#include "bench.h"
#include "stdio.h"
#include "custom_psend.h"

void bench_custom_psend(TestCase *test_case, Result *result, int comm_rank)
{
	MPI_Barrier(MPI_COMM_WORLD);

	// init
	custom_MPI_Request request;

	timer* timers;
	timers_init(&timers, test_case, result);

	if (comm_rank == 0)
	{
		custom_MPI_Psend_init(test_case->buffer, test_case->buffer_size / test_case->partition_size, test_case->partition_size, MPI_CHAR, 1, 0, MPI_COMM_WORLD, MPI_INFO_ENV, &request);
	}
	else if (comm_rank == 1)
	{
		custom_MPI_Precv_init(test_case->buffer, test_case->buffer_size / test_case->partition_size, test_case->partition_size, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_INFO_ENV, &request);
	}

	// run
	MPI_Barrier(MPI_COMM_WORLD);
	timers_start_global(timers);

	for (size_t i = 0; i < test_case->iteration_count; i++)
	{
		if (comm_rank == 0)
		{
			timers_start_local(timers);

			custom_MPI_Start(&request);

			for (size_t p = 0; p < test_case->partition_count; p++) {
				unsigned int partition_num = test_case->send_pattern[p];
				work(test_case->partition_size);
				custom_MPI_Pready(partition_num, &request);
			}

			custom_MPI_Wait(&request, &result->send_status);

			timers_stop_local(timers);
		}
		else if (comm_rank == 1)
		{
			timers_start_local(timers);

			custom_MPI_Start(&request);
			custom_MPI_Wait(&request, &result->recv_status);

			timers_stop_local(timers);
		}
	}

	MPI_Barrier(MPI_COMM_WORLD);
	timers_stop_global(timers);

	timers_store(timers, result);
	timers_free(timers);

	custom_MPI_Free(&request);
};