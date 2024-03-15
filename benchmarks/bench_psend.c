#include "bench.h"
#include "benchmarks/benchmark.h"

/*
using ob1, fails with

[r34c4t6n2:685031:0:685031] Caught signal 11 (Segmentation fault: address not mapped to object at address 0x10)
==== backtrace (tid: 685031) ====
 0 0x0000000000012cf0 __funlockfile()  :0
 1 0x000000000028dd20 mca_pml_ob1_recv_request_progress_rget()  ???:0
 2 0x000000000029167d mca_pml_ob1_recv_req_start()  ???:0
 3 0x00000000002985d8 mca_pml_ob1_start()  ???:0
 4 0x000000000027282a mca_part_persist_start()  part_persist_recvreq.c:0
 5 0x00000000000e2f7d MPI_CHECK(MPI_Start()  ???:0
 6 0x0000000000402f65 bench_psend()  ???:0
 7 0x0000000000406951 bench()  ???:0
 8 0x0000000000401922 main()  ???:0
 9 0x000000000003ad85 __libc_start_main()  ???:0
10 0x0000000000401c4e _start()  ???:0

mca_part_persist_start only calls other start function if req->req_type != MCA_PART_PERSIST_REQUEST_PSEND
maybe req->req_type is set incorrectly?

*/



void bench_psend(TestCase *test_case, Result *result, int comm_rank)
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