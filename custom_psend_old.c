#include "custom_psend.h"

#include <mpi.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <semaphore.h>


typedef struct {
    size_t capacity;
    char* buffer;

    size_t head;
    size_t tail;
} queue;



struct custom_MPI_Request
{
    int other_rank;
    int is_sender;

    void *buffer;
    size_t buffer_size;

    int partition_count;
    int partition_size;

    char *ready_partitions;

    MPI_Win data_window;
    MPI_Win ready_partition_window;

    queue partition_queue;
    pthread_t worker;
    sem_t queue_num_elements;
    sem_t queue_remaining_capacity;
};
typedef struct custom_MPI_Request* custom_MPI_Request;



#define STATUS {}
    // int rank; MPI_Comm_rank(MPI_COMM_WORLD, &rank); printf("Rank %i at line %i\n", rank, __LINE__); fflush(stdout);}


void queue_init(queue* q, void* buf, size_t capacity) {
    q->capacity = capacity;
    q->buffer = buf;
    q->head = 0;
    q->tail = 0;
}

size_t queue_size(queue* q) {
    return (q->tail - q->head + q->capacity) % q->capacity;
}

void queue_push(queue* q, const void* elements, size_t size) {
    void* dest = (char*)q->buffer + q->tail;
    if (q->tail <= q->capacity - size) {
        memcpy(dest, elements, size);
    } else {
        size_t first = (q->capacity - q->tail);
        size_t second = size - first;
        memcpy(dest, elements, first);
        memcpy(q->buffer, (char*)elements + first, second);
    }
    q->tail = (q->tail + size) % q->capacity;
}

void queue_pull(queue* q, void* elements, size_t size) {
    char* from = ((char*)q->buffer) + q->head;
    if (q->head <= q->capacity - size) {
        memcpy(elements, from, size);
    } else {
        size_t first = (q->capacity - q->head);
        size_t second = size - first;
        memcpy(elements, from, first);
        memcpy((char*)elements + first, q->buffer, second);
    }
    q->head = (q->head + size) % q->capacity;
}

int custom_MPI_Precv_init(void *buf, int partitions, MPI_Count count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Info info, custom_MPI_Request* request)
{
    struct custom_MPI_Request* result = malloc(sizeof(struct custom_MPI_Request));
    int type_size;
    MPI_Type_size(datatype, &type_size);

    result->other_rank = source;
    result->is_sender = 0;

    result->partition_count = partitions;
    result->partition_size = count;
    result->ready_partitions = malloc(partitions);

    result->buffer = buf;
    result->buffer_size = partitions * count * type_size;

    MPI_Win_create(result->ready_partitions, result->partition_count, 1, MPI_INFO_ENV, MPI_COMM_WORLD, &result->ready_partition_window);
    MPI_Win_create(result->buffer, result->buffer_size, 1, MPI_INFO_ENV, MPI_COMM_WORLD, &result->data_window);

    queue_init(&result->partition_queue, NULL, 0);

    sem_init(&result->queue_num_elements, 0, 0);
    sem_init(&result->queue_remaining_capacity, 0, 0);

    *request = result;
    return 0;
}

int custom_MPI_Psend_init(const void *buf, int partitions, MPI_Count count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Info info, custom_MPI_Request *request)
{
    struct custom_MPI_Request* result = malloc(sizeof(struct custom_MPI_Request));
    result->other_rank = dest;
    result->is_sender = 1;

    int type_size;
    MPI_Type_size(datatype, &type_size);

    result->partition_count = partitions;
    result->ready_partitions = malloc(partitions);
    result->partition_size = count;

    result->buffer = (void*) buf;
    result->buffer_size = partitions * count * type_size;

    MPI_Win_create(result->ready_partitions, result->partition_count, 1, MPI_INFO_ENV, MPI_COMM_WORLD, &result->ready_partition_window);
    MPI_Win_create(result->buffer, result->buffer_size, 1, MPI_INFO_ENV, MPI_COMM_WORLD, &result->data_window);

    queue_init(&result->partition_queue, malloc((partitions + 2) * sizeof(int)), (partitions + 1) * sizeof(int));

    *request = result;

    return 0;
}

int custom_MPI_Parrived(custom_MPI_Request request, int partition, int *flag)
{
    *flag = (int)request->ready_partitions[partition];
    return 0;
}

int custom_MPI_Pready(int partition, custom_MPI_Request request)
{
    queue_push(&request->partition_queue, &partition, sizeof(int));
    sem_post(&request->queue_num_elements);

    return 0;
}

int custom_MPI_Pready_list(int length, const int array_of_partitions[], custom_MPI_Request request)
{
    return -1;
}

void* work_send(void* r)
{
    custom_MPI_Request request = r;
   
    int partitions_sent = 0;

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    while (partitions_sent != request->partition_count)
    {
        sem_wait(&request->queue_num_elements);

        size_t backoff = 1000;
        while(queue_size(&request->partition_queue) < sizeof(int)) 
            usleep(backoff);

        int partition;
        queue_pull(&request->partition_queue, &partition, sizeof(int));

        // partitions can be added to queue
        sem_post(&request->queue_remaining_capacity);

        // write partition to buffer of other rank
        MPI_Win_lock(MPI_LOCK_SHARED, request->other_rank, 0, request->data_window);
        void* address = (char*)request->buffer + (partition * request->partition_size);
        MPI_Put(address, request->partition_size, MPI_BYTE, request->other_rank, request->partition_size * partition, request->partition_size, MPI_BYTE, request->data_window);
        MPI_Win_unlock(request->other_rank, request->data_window);

        // mark partition as sent
        char ready = 1;
        MPI_Win_lock(MPI_LOCK_EXCLUSIVE, request->other_rank, 0, request->ready_partition_window);
        MPI_Put(&ready, 1, MPI_BYTE, request->other_rank, partition, 1, MPI_BYTE, request->ready_partition_window);
        MPI_Win_unlock(request->other_rank, request->ready_partition_window);

        partitions_sent += 1;
    }

    return NULL;
}

void* work_recv(void* r)
{
    custom_MPI_Request request = r;
    return NULL;
}

int custom_MPI_Start(custom_MPI_Request request)
{
    if (request->is_sender == 1) {
        sem_init(&request->queue_num_elements, 0, 0);
        sem_init(&request->queue_remaining_capacity, 0, request->partition_count);
        pthread_create(&request->worker, NULL, &work_send, request);
    } else {
        memset(request->ready_partitions, 0, request->partition_count);
        pthread_create(&request->worker, NULL, &work_recv, request);
    }

    return 0;
}

int custom_MPI_Wait(custom_MPI_Request request, MPI_Status* status)
{
    void* result;
    pthread_join(request->worker, &result);

    MPI_Barrier(MPI_COMM_WORLD);

    int ret = 0;
    return ret;
}


int custom_MPI_Free(custom_MPI_Request* request) {

    MPI_Win_free(&(*request)->data_window);
    MPI_Win_free(&(*request)->ready_partition_window);

    free((*request)->partition_queue.buffer);
    free((*request)->ready_partitions);

    *request = NULL;
    return 0;
}