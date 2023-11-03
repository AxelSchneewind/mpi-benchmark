#include "bench.h"
#include "stdio.h"

#include <semaphore.h>
#include <pthread.h>


//#define STATUS {printf("%i\n", __LINE__); fflush(stdout); }
#define STATUS {}

typedef struct
{
	pthread_t thread;
	volatile MPI_Request *request;
	sem_t started;
	sem_t signal_done;
	sem_t done;
} progress_thread;

void *progress(void *arguments)
{
	progress_thread *thread = (progress_thread *)arguments;

	while (1)
	{
		STATUS;
		sem_wait(&thread->started);
		STATUS;

		int ret = 0;
		int done = 0;
		int flag = 0;

		MPI_Status status;
		do
		{
			ret = MPI_Request_get_status(*thread->request, &flag, &status);
			sem_getvalue(&thread->signal_done, &done);
		} while (done == 0 && ret == 0);

		sem_post(&thread->done);

		while(done > 0) {
			sem_wait(&thread->signal_done);
			sem_getvalue(&thread->signal_done, &done);
		}
	}

	return NULL;
}

void progress_thread_create(progress_thread *thread)
{
	STATUS;
	thread->request = NULL;
	sem_init(&thread->started, 0, 0);
	sem_init(&thread->done, 0, 0);
	sem_init(&thread->signal_done, 0, 0);
	STATUS;

	pthread_create(&thread->thread, 0, progress, thread);
	STATUS;
};
void progress_thread_set_request(progress_thread *thread, MPI_Request *request)
{
	STATUS;
	thread->request = request;

	sem_post(&thread->started);
	STATUS;
};
void progress_thread_wait(progress_thread *thread)
{
	STATUS;
	sem_post(&thread->signal_done);
	sem_wait(&thread->done);
	STATUS;
};

void bench_psend_progress(TestCase *test_case, Result *result, int comm_rank)
{
	// init
	MPI_Request send_request;
	MPI_Request recv_request;

	timer *timers;
	timers_init(&timers, test_case, result);

	if (comm_rank == 0)
	{
		MPI_Psend_init(test_case->buffer, test_case->partition_count, test_case->partition_size, MPI_CHAR, 1, 0, MPI_COMM_WORLD, MPI_INFO_ENV, &send_request);
	}
	else if (comm_rank == 1)
	{
		MPI_Precv_init(test_case->buffer, test_case->partition_count_recv, test_case->partition_size_recv, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_INFO_ENV, &recv_request);
	}

	progress_thread progress;
	progress_thread_create(&progress);

	usleep(1000);

	// run
	MPI_Barrier(MPI_COMM_WORLD);
	timers_start_global(timers);

	for (size_t i = 0; i < test_case->iteration_count; i++)
	{
		if (comm_rank == 0)
		{
			timers_start_local(timers);

			MPI_Start(&send_request);

			progress_thread_set_request(&progress, &send_request);

			for (size_t p = 0; p < test_case->partition_count; p++)
			{
				unsigned int partition_num = test_case->send_pattern[p];
				work(test_case->partition_size);
				MPI_Pready(partition_num, send_request);
			}

			progress_thread_wait(&progress);

			MPI_Wait(&send_request, &result->send_status);

			timers_stop_local(timers);
		}
		else if (comm_rank == 1)
		{
			timers_start_local(timers);

			MPI_Start(&recv_request);
			MPI_Wait(&recv_request, &result->recv_status);

			timers_stop_local(timers);
		}
	}

	MPI_Barrier(MPI_COMM_WORLD);
	timers_stop_global(timers);

	timers_store(timers, result);
	timers_free(timers);
};