#include "bench.h"

#include <stdio.h>
#include <stdatomic.h>
#include <semaphore.h>
#include <pthread.h>

// #define ENABLE_STATUS ;

#ifdef ENABLE_STATUS
	#define STATUS {printf("%i\n", __LINE__); fflush(stdout); }
	#define STATUS_RANK {printf("%i: %i\n", comm_rank, __LINE__); fflush(stdout); }
#else
	#define STATUS {}
	#define STATUS_RANK {}
#endif

typedef struct
{
	pthread_t thread;
	_Atomic(MPI_Request*) request;
	sem_t request_available;
	sem_t signal_done;
	sem_t accept_requests;
} progress_thread;

void *progress(void *arguments)
{
	progress_thread *thread = (progress_thread *)arguments;
	sem_post(&thread->accept_requests);

	while (1)
	{
		// STATUS;
		sem_wait(&thread->request_available);
		MPI_Request* request = atomic_load(&thread->request);
		if (request == NULL)
			break;
		// STATUS;

		int ret = 0;
		int ret_trywait = 0;
		int flag = 0;

		MPI_Status status;
		do
		{
			ret = MPI_Request_get_status(*request, &flag, &status);
			ret_trywait = sem_trywait(&thread->signal_done);
		} while (ret_trywait < 0 && ret == 0 && !flag); // exit if MPI call failed or signal_done is posted

		sem_post(&thread->accept_requests);
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
};

void progress_thread_destroy(progress_thread* thread) {
	progress_thread_set_request(thread, NULL);

	pthread_join(thread->thread, NULL);

	sem_destroy(&thread->signal_done);
	sem_destroy(&thread->accept_requests);
	sem_destroy(&thread->request_available);
}


void progress_thread_set_request(progress_thread *thread, MPI_Request *request)
{
	sem_wait(&thread->accept_requests);
	atomic_store(&thread->request, request);
	sem_post(&thread->request_available);
};
void progress_thread_wait(progress_thread *thread)
{
	sem_post(&thread->signal_done);
};

void bench_psend_progress(TestCase *test_case, Result *result, int comm_rank)
{
	// init
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

	progress_thread progress;
	progress_thread_create(&progress);

	// run
	MPI_Barrier(MPI_COMM_WORLD);
	timers_start_global(timers);

	for (size_t i = 0; i < test_case->iteration_count; i++)
	{
		if (comm_rank == 0)
		{
			timers_start_local(timers);

			STATUS_RANK;
			MPI_Start(&request);

			progress_thread_set_request(&progress, &request);
			STATUS_RANK;

			for (size_t p = 0; p < test_case->partition_count; p++)
			{
				unsigned int partition_num = test_case->send_pattern[p];
				work(test_case->partition_size);
				MPI_Pready(partition_num, request);
			}

			progress_thread_wait(&progress);
			STATUS_RANK;

			MPI_Wait(&request, &result->send_status);
			STATUS_RANK;

			timers_stop_local(timers);
		} else if (comm_rank == 1) {
			timers_start_local(timers);

			STATUS_RANK;
			MPI_Start(&request);

			progress_thread_set_request(&progress, &request);

			progress_thread_wait(&progress);
			STATUS_RANK;

			MPI_Wait(&request, &result->recv_status);
			STATUS_RANK;

			timers_stop_local(timers);
		}
	}

	STATUS_RANK;
	MPI_Barrier(MPI_COMM_WORLD);
	timers_stop_global(timers);

	STATUS_RANK;
	progress_thread_destroy(&progress);
	STATUS_RANK;

	timers_store(timers, result);
	timers_free(timers);
};