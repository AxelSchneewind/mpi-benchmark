#include "bench.h"
#include "stdio.h"

void bench_isend_then_test(TestCase *test_case, Result *result, int comm_rank)
{
	MPI_Request *requests = malloc(sizeof(MPI_Request) * test_case->partition_count);
	timers timers;
	timers_init(&timers, TimerCount);

    // warmup
    if (comm_rank == 0) {
        MPI_CHECK(MPI_Isend(test_case->buffer, test_case->buffer_size, MPI_BYTE, 1, 0, MPI_COMM_WORLD, &requests[0]));
		MPI_CHECK(MPI_Wait(&requests[0], MPI_STATUSES_IGNORE));
    } else {
        MPI_CHECK(MPI_Irecv(test_case->buffer, test_case->buffer_size, MPI_BYTE, 0, 0, MPI_COMM_WORLD, &requests[0]));
		MPI_CHECK(MPI_Wait(&requests[0], MPI_STATUSES_IGNORE));
    }


	MPI_CHECK(MPI_Barrier(MPI_COMM_WORLD));
	timers_start(timers, Total);

	for (size_t i = 0; i < test_case->iteration_count; i++)
	{
		if (comm_rank == 0)
		{
			timers_start(timers, Iteration);
			timers_start(timers, IterationStartToWait);

			for (MPI_Count p = 0; p < test_case->partition_count; p++)
			{
				unsigned int partition_num = test_case->send_pattern[p];
				work(test_case->partition_size);

				MPI_CHECK(MPI_Isend(test_case->buffer + partition_num * test_case->partition_size, test_case->partition_size, MPI_BYTE, 1, 0, MPI_COMM_WORLD, &requests[partition_num]));
			}

			for (MPI_Count p = 0; p < test_case->partition_count; p++){
				unsigned int partition_num = p; // test_case->send_pattern[p];				
				int flag;
				MPI_CHECK(MPI_Test(&requests[partition_num], &flag, &result->send_status));
			}

			timers_stop(timers, IterationStartToWait);
			MPI_CHECK(MPI_Waitall(test_case->partition_count, requests, MPI_STATUSES_IGNORE));

			timers_stop(timers, Iteration);
		}
		else if (comm_rank == 1)
		{
			timers_start(timers, Iteration);
			timers_start(timers, IterationStartToWait);

			for (size_t p = 0; p < test_case->partition_count; p++) {
				unsigned int partition_num = test_case->recv_pattern[p];
				MPI_CHECK(MPI_Irecv(test_case->buffer + partition_num * test_case->partition_size, test_case->partition_size, MPI_BYTE, 0, 0, MPI_COMM_WORLD, &requests[partition_num]));
			}

			timers_stop(timers, IterationStartToWait);
			MPI_CHECK(MPI_Waitall(test_case->partition_count, requests, MPI_STATUSES_IGNORE));

			timers_stop(timers, Iteration);
		}
	}

	MPI_CHECK(MPI_Barrier(MPI_COMM_WORLD));
	timers_stop(timers, Total);

	timers_store(timers, result);
	timers_free(timers);

	free(requests);
};