#include "bench.h"

void bench_psend_parrived(TestCase *test_case, Result *result, int comm_rank)
{
	// init requests and timer
	MPI_Request request;

	timer *timers;
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

	if (comm_rank == 0)
	{
		for (size_t i = 0; i < test_case->iteration_count; i++)
		{
			timers_start_local(timers);

			MPI_Start(&request);

			for (size_t p = 0; p < test_case->partition_count; p++)
			{
				unsigned int partition_num = test_case->send_pattern[p];
				work(test_case->partition_size);
				MPI_Pready(partition_num, request);
			}

			MPI_Wait(&request, &result->send_status);

			timers_stop_local(timers);
		}
	}
	else if (comm_rank == 1)
	{
		for (size_t i = 0; i < test_case->iteration_count; i++)
		{
			timers_start_local(timers);

			MPI_Start(&request);

			int flag = 0;
			while (!flag)
			{
				flag = 1;
				for (size_t p = 0; p < test_case->partition_count_recv; p++)
				{
				    int _flag = 0;
				    MPI_Request_get_status(request, &_flag, &result->recv_status);
				    flag = flag && _flag;
					if (flag)		// somehow deadlocks without this (in openmpi)
					    break;
					MPI_Parrived(request, p, &_flag);
					flag = flag && _flag;
				}
			}

			MPI_Wait(&request, &result->recv_status);

			timers_stop_local(timers);
		}
	}

	MPI_Barrier(MPI_COMM_WORLD);
	timers_stop_global(timers);

	MPI_Request_free(&request);

	timers_store(timers, result);
	timers_free(timers);
};
