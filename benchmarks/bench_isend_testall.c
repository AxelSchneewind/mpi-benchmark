#include "bench.h"
#include "stdio.h"

void bench_isend_testall(TestCase *test_case, Result *result, int comm_rank)
{
	MPI_Request *requests = malloc(sizeof(MPI_Request) * test_case->partition_count);
	MPI_Status *statuses = malloc(sizeof(MPI_Status) * test_case->partition_count);

	timer* timers;
	timers_init(&timers, test_case, result);


	MPI_CHECK(MPI_Barrier(MPI_COMM_WORLD));
	timers_start_global(timers);

	for (size_t i = 0; i < test_case->iteration_count; i++)
	{
		if (comm_rank == 0)
		{
			timers_start_local(timers);

			for (MPI_Count p = 0; p < test_case->partition_count; p++)
			{
				unsigned int partition_num = test_case->send_pattern[p];				work(test_case->partition_size);

				MPI_CHECK(MPI_Isend(test_case->buffer + partition_num * test_case->partition_size, test_case->partition_size, MPI_BYTE, 1, 0, MPI_COMM_WORLD, &requests[partition_num]));
			}

			int flag;
			MPI_CHECK(MPI_Testall(test_case->partition_count, requests, &flag, statuses));

			MPI_CHECK(MPI_Waitall(test_case->partition_count, requests, MPI_STATUSES_IGNORE));

			timers_stop_local(timers);
		}
		else if (comm_rank == 1)
		{
			timers_start_local(timers);

			for (size_t p = 0; p < test_case->partition_count; p++) {
				unsigned int partition_num = test_case->send_pattern[p];				
				MPI_CHECK(MPI_Irecv(test_case->buffer + partition_num * test_case->partition_size, test_case->partition_size, MPI_BYTE, 0, 0, MPI_COMM_WORLD, &requests[partition_num]));
			}

			MPI_CHECK(MPI_Waitall(test_case->partition_count, requests, MPI_STATUSES_IGNORE));

			timers_stop_local(timers);
		}
	}

	MPI_CHECK(MPI_Barrier(MPI_COMM_WORLD));
	timers_stop_global(timers);

	timers_store(timers, result);
	timers_free(timers);

	free(statuses);
	free(requests);
};