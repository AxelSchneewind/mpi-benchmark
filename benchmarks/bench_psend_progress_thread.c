#include "bench.h"

#include <stdio.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>

#include <assert.h>

#define ENABLE_STATUS 0

#if ENABLE_STATUS == 1
#define OUTPUT(x)                         \
	{                                     \
		x;                                \
		printf("%i, R0: " #x "\n", __LINE__); \
		assert(0 == fflush(stdout));      \
	}

#define OUTPUT_RANK(x)                         \
	{                                     \
		x;                                \
		printf("%i, R%i: " #x "\n", __LINE__, comm_rank); \
		assert(0 == fflush(stdout));      \
	}
#else
#define OUTPUT(x) \
	{             \
		x;        \
	}

#define OUTPUT_RANK(x) { x; }

#endif

typedef struct
{
	pthread_t thread;
	volatile MPI_Request *request;
	sem_t request_available;
	sem_t signal_done;
	sem_t accept_requests;
} progress_thread;

#if ENABLE_STATUS == 1
#define progress_thread_check(thread)                                                                      \
	{                                                                                                      \
		int request_available_value;                                                                       \
		int signal_done_value;                                                                             \
		int accept_requests_value;                                                                         \
                                                                                                           \
		sem_getvalue(&thread->request_available, &request_available_value);                                \
		sem_getvalue(&thread->signal_done, &signal_done_value);                                            \
		sem_getvalue(&thread->accept_requests, &accept_requests_value);                                    \
                                                                                                           \
		assert(request_available_value <= 1);                                                              \
		assert(signal_done_value <= 1);                                                                    \
		assert(accept_requests_value <= 1);                                                                \
	}
#else
#define progress_thread_check(thread) ;
#endif

void *progress(void *arguments)
{
	progress_thread *thread = (progress_thread *)arguments;
	sem_post(&thread->accept_requests);
	progress_thread_check(thread);

	while (1)
	{
		int ret_trywait = 0;
		int flag = 0;
		MPI_Status status;

		OUTPUT(sem_wait(&thread->request_available));
		OUTPUT(sem_wait(&thread->accept_requests));
		if (thread->request == NULL)
			break;
		progress_thread_check(thread);

		ret_trywait = sem_trywait(&thread->signal_done);
		while (ret_trywait != 0) // exit if signal_done is posted
		{
			// MPI_Request_get_status(*thread->request, &flag, &status);
			usleep(100);
			ret_trywait = sem_trywait(&thread->signal_done);
		}

		OUTPUT(sem_post(&thread->accept_requests));
		progress_thread_check(thread);
	}

	return NULL;
}

void progress_thread_create(progress_thread *thread)
{
	thread->request = NULL;
	sem_init(&thread->request_available, 0, 0);
	sem_init(&thread->accept_requests, 0, 0);
	sem_init(&thread->signal_done, 0, 0);

	pthread_create(&thread->thread, 0, progress, thread);
	progress_thread_check(thread);
};

void progress_thread_set_request(progress_thread *thread, MPI_Request *request)
{
	progress_thread_check(thread);
	sem_wait(&thread->accept_requests);
	thread->request = request;
	sem_post(&thread->accept_requests);
	progress_thread_check(thread);
};

void progress_thread_pause(progress_thread *thread)
{
	progress_thread_check(thread);
	sem_post(&thread->signal_done);

	sem_wait(&thread->accept_requests);
	sem_post(&thread->accept_requests);

	progress_thread_check(thread);
};

void progress_thread_continue(progress_thread *thread)
{
	progress_thread_check(thread);
	sem_trywait(&thread->request_available);
	sem_post(&thread->request_available);
	progress_thread_check(thread);
};

void progress_thread_destroy(progress_thread *thread)
{
	progress_thread_check(thread);
	progress_thread_set_request(thread, NULL);
	progress_thread_continue(thread);

	pthread_join(thread->thread, NULL);

	sem_destroy(&thread->signal_done);
	sem_destroy(&thread->accept_requests);
	sem_destroy(&thread->request_available);
}

/*
In openmpi on hawk and ucx, crashes with error: 
	[r40c1t8n1:3249792:0:3249792] Caught signal 11 (Segmentation fault: address not mapped to object at address 0x1)
	==== backtrace (tid:3249792) ====
	 0 0x0000000000012cf0 __funlockfile()  :0
	 1 0x00000000002796be mca_pml_ucx_psend_completion()  ???:0
	 2 0x000000000007606d ucp_rndv_ats_handler()  ???:0
	 3 0x000000000003f319 uct_rc_mlx5_iface_check_rx_completion()  ???:0
	 4 0x000000000004890a ucp_worker_progress()  ???:0
	 5 0x00000000000276e3 opal_progress()  ???:0
	 6 0x000000000005fb25 ompi_sync_wait_mt()  ???:0
	 7 0x000000000009795f ompi_request_default_wait()  ???:0
	 8 0x00000000000e8ffe MPI_Wait()  ???:0
	 9 0x0000000000403cbb bench_psend_progress_thread()  ???:0
	10 0x0000000000406931 bench()  ???:0
	11 0x0000000000401905 main()  ???:0
	12 0x000000000003ad85 __libc_start_main()  ???:0
	13 0x0000000000401c7e _start()  ???:0
	=================================
*/
void bench_psend_progress_thread(TestCase *test_case, Result *result, int comm_rank)
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

	progress_thread progress;
	progress_thread_create(&progress);
	progress_thread_set_request(&progress, &request);

	if (comm_rank == 0)
	{
		for (size_t i = 0; i < test_case->iteration_count; i++)
		{
			timers_start(timers, Iteration);
			timers_start(timers, IterationStartToWait);

			MPI_CHECK(MPI_Start(&request));
#if ENABLE_STATUS == 1
			printf("%d, R0: MPI_Start(&request);        iteration %d\n", __LINE__, i);
			assert(0 == fflush(stdout));
#endif
		 	OUTPUT_RANK(progress_thread_set_request(&progress, &request));
		 	OUTPUT_RANK(progress_thread_continue(&progress));

			for (size_t p = 0; p < test_case->partition_count; p++)
			{
				unsigned int partition_num = test_case->send_pattern[p];
				work(test_case->partition_size);
				MPI_CHECK(MPI_Pready(partition_num, request));
			}


			OUTPUT_RANK(progress_thread_pause(&progress));
			timers_stop(timers, IterationStartToWait);
			OUTPUT_RANK(MPI_CHECK(MPI_Wait(&request, &result->send_status)));

			timers_stop(timers, Iteration);
		}

	}
	else if (comm_rank == 1)
	{
		for (size_t i = 0; i < test_case->iteration_count; i++)
		{
			timers_start(timers, Iteration);
			timers_start(timers, IterationStartToWait);

			MPI_CHECK(MPI_Start(&request));
#if ENABLE_STATUS == 1
			printf("%d, R1: MPI_Start(&request);        iteration %d\n", __LINE__, i);
			assert(0 == fflush(stdout));
#endif

            int flag = false;
            while(!flag)
            {
                MPI_CHECK(MPI_Request_get_status(request, &flag, MPI_STATUS_IGNORE));
            }

			timers_stop(timers, IterationStartToWait);
			OUTPUT_RANK(MPI_CHECK(MPI_Wait(&request, &result->recv_status)));

			timers_stop(timers, Iteration);
		}
	}

	MPI_CHECK(MPI_Barrier(MPI_COMM_WORLD));
	timers_stop(timers, Total);

	OUTPUT_RANK(progress_thread_destroy(&progress));
	MPI_CHECK(MPI_Barrier(MPI_COMM_WORLD));

	OUTPUT_RANK(MPI_CHECK(MPI_Request_free(&request)));

	timers_store(timers, result);
	timers_free(timers);
};
