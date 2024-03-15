#include "bench.h"
#include "benchmarks/benchmark.h"

void bench_psend_list(TestCase *test_case, Result *result, int comm_rank)
{
	// init
	MPI_Request request;

	timers timers;
	timers_init(&timers, TimerCount);

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

		MPI_Pready_list(test_case->partition_count, test_case->send_pattern, request);

        MPI_CHECK(MPI_Wait(&request, &result->send_status));
    } else if (comm_rank == 1) {
        MPI_CHECK(MPI_Start(&request));
        MPI_CHECK(MPI_Wait(&request, &result->recv_status));
    }

	// run
	MPI_CHECK(MPI_Barrier(MPI_COMM_WORLD));
	timers_start(timers, Total);

	if (comm_rank == 0)
	{
		for (size_t i = 0; i < test_case->iteration_count; i++)
		{
			PreIteration;
			timers_start(timers, Iteration);
			timers_start(timers, IterationStartToWait);

			MPI_CHECK(MPI_Start(&request));

			for (size_t p = 0; p < test_case->partition_count; p++)
			{
				unsigned int partition_num = test_case->send_pattern[p];
				work(test_case->partition_size);
				MPI_CHECK(MPI_Pready(partition_num, request));
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
			PreIteration;
			timers_start(timers, Iteration);
			timers_start(timers, IterationStartToWait);

			MPI_CHECK(MPI_Start(&request));

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