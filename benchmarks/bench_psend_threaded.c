#include "bench.h"
#include "stdio.h"
#include <semaphore.h>
#include <pthread.h>

#define STATUS                                          \
	{                                                   \
		int rank;                                       \
		MPI_Comm_rank(MPI_COMM_WORLD, &rank);           \
		printf("Rank %i at line %i\n", rank, __LINE__); \
		fflush(stdout);                                 \
	}

struct _thread_args
{
	TestCase *test_case;
	Result *result;
	MPI_Request *request;
	int comm_rank;
	timer *timers;
	sem_t *ready;
	sem_t *done;
	int thread_num;
	int thread_count;
};
typedef struct _thread_args thread_args;

void bench_send_thread(TestCase *test_case, Result *result, MPI_Request *request, int comm_rank, timer *timers, int thread_num, int thread_count)
{
	//timers_start_local(timers);

	int partitions_per_thread = test_case->partition_count / thread_count;
	for (size_t p = 0; p < partitions_per_thread; p++)
	{
		unsigned int partition_num = test_case->send_pattern[p + thread_num * partitions_per_thread];
		work(test_case->partition_size);
		MPI_Pready(partition_num, *request);
	}

	//timers_stop_local(timers);
}
void bench_recv_thread(TestCase *test_case, Result *result, MPI_Request *request, int comm_rank, timer *timers, int thread_num, int thread_count)
{
	//timers_start_local(timers);

	for (size_t p = 0; p < test_case->partition_count / thread_count; p++)
	{
	}

	//timers_stop_local(timers);
};

void *run_send_thread(void *_args)
{
	thread_args *args = _args;
	for (size_t i = 0; i < args->test_case->iteration_count; i++)
	{
		sem_wait(args->ready);
		bench_send_thread(args->test_case, args->result, args->request, args->comm_rank, args->timers, args->thread_num, args->thread_count);
		sem_post(args->done);
	}

	return NULL;
};

void *run_recv_thread(void *_args)
{
	thread_args *args = _args;
	for (size_t i = 0; i < args->test_case->iteration_count; i++)
	{
		sem_wait(args->ready);
		bench_recv_thread(args->test_case, args->result, args->request, args->comm_rank, args->timers, args->thread_num, args->thread_count);
		sem_post(args->done);
	}

	return NULL;
};

void bench_psend_threaded(TestCase *test_case, Result *result, int comm_rank)
{
	timer *timers;
	timers_init(&timers);

	MPI_Request request;

	int thread_count = 8;
	if (test_case->partition_count < thread_count)
		thread_count = test_case->partition_count;

	pthread_t *threads = malloc(sizeof(pthread_t) * thread_count);
	thread_args *args = malloc(sizeof(thread_args) * thread_count);

	sem_t *start_thread = malloc(sizeof(sem_t) * thread_count);
	sem_t *thread_done = malloc(sizeof(sem_t) * thread_count);
	for (size_t i = 0; i < thread_count; i++)
	{
		sem_init(&start_thread[i], 0, 0);
		sem_init(&thread_done[i], 0, 0);
	}

	if (comm_rank == 0)
	{
		MPI_Psend_init(test_case->buffer, test_case->partition_count, test_case->partition_size, MPI_CHAR, 1, 0, MPI_COMM_WORLD, MPI_INFO_ENV, &request);
		for (size_t i = 0; i < thread_count; i++)
		{
			args[i] = (thread_args){
				.test_case = test_case,
				.result = result,
				.comm_rank = comm_rank,
				.timers = timers,
				.ready = &start_thread[i],
				.done = &thread_done[i],
				.thread_num = i,
				.request = &request,
				.thread_count = thread_count};

			pthread_create(&threads[i], 0, run_send_thread, &args[i]);
		}
	}
	else
	{
		MPI_Precv_init(test_case->buffer, test_case->partition_count_recv, test_case->partition_size_recv, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_INFO_ENV, &request);
		for (size_t i = 0; i < thread_count; i++)
		{
			args[i] = (thread_args){
				.test_case = test_case,
				.result = result,
				.comm_rank = comm_rank,
				.timers = timers,
				.ready = &start_thread[i],
				.done = &thread_done[i],
				.thread_num = i,
				.request = &request,
				.thread_count = thread_count};

			pthread_create(&threads[i], 0, run_recv_thread, &args[i]);
		}
	}

	MPI_Barrier(MPI_COMM_WORLD);
	timers_start(timers, Total);

	for (size_t i = 0; i < args->test_case->iteration_count; i++)
	{
		timers_start(timers, Iteration);
		timers_start(timers, IterationStartToWait);
		MPI_Start(&request);
		for (size_t i = 0; i < thread_count; i++)
			sem_post(&start_thread[i]);
		for (size_t i = 0; i < thread_count; i++)
			sem_wait(&thread_done[i]);
		timers_stop(timers, IterationStartToWait);
		MPI_Wait(&request, MPI_STATUS_IGNORE);
		timers_stop(timers, Iteration);
	}

	for (size_t i = 0; i < thread_count; i++)
		pthread_join(threads[i], NULL);

	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Request_free(&request);
	timers_stop(timers, Total);

	timers_store(timers, result);
	timers_free(timers);

	free(start_thread);
	free(thread_done);
	free(threads);
	free(args);
};
