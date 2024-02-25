#include <mpi.h>
#include <pthread.h>
#include <semaphore.h>
#include "interval_tree.h"

struct custom_MPI_Request
{
    int destination;
    int is_sender;

    int partition_size;
    int aggregation_factor;

    char* buffer;
    MPI_Count buffer_size;

    MPI_Win window;
    tree ready_intervals;
};
typedef struct custom_MPI_Request custom_MPI_Request;

int custom_MPI_Start(custom_MPI_Request* request);
int custom_MPI_Wait(custom_MPI_Request* request, MPI_Status* status);
int custom_MPI_Free(custom_MPI_Request* request);

int custom_MPI_Precv_init(void *buf, int partitions, MPI_Count count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Info info, custom_MPI_Request *request);
int custom_MPI_Psend_init(const void *buf, int partitions, MPI_Count count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Info info, custom_MPI_Request *request);
int custom_MPI_Parrived(custom_MPI_Request* request, int partition, int *flag);
int custom_MPI_Pready(int partition, custom_MPI_Request* request);
int custom_MPI_Pready_list(int length, const int array_of_partitions[], custom_MPI_Request* request);

int custom_MPI_Start(custom_MPI_Request* request) {
    return 0;
};
int custom_MPI_Wait(custom_MPI_Request* request, MPI_Status* status) {
    MPI_Win_lock(MPI_LOCK_SHARED, request->destination, MPI_MODE_NOCHECK, request->window);
    int min, max;
    int size = request->aggregation_factor;
    while (size >= 1) {
        find_interval(request->ready_intervals, size, &min, &max);
        while (max - min >= size) {
            MPI_Put(&request->buffer[request->partition_size * min], request->partition_size * request->aggregation_factor, MPI_CHAR, request->destination, request->partition_size * min, request->partition_size * request->aggregation_factor, MPI_CHAR, request->window);
            marks_remove(request->ready_intervals, min, max);
            find_interval(request->ready_intervals, size, &min, &max);
        }
        size--;
    }
    MPI_Win_unlock(request->destination, request->window);
    return 0;
};
int custom_MPI_Free(custom_MPI_Request* request) {
    MPI_Win_free(&request->window);
};

int custom_MPI_Precv_init(void *buf, int partitions, MPI_Count count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Info info, custom_MPI_Request *request) {
    request->buffer = (char*)buf;
    request->buffer_size = count * partitions;
    request->destination = source;
    MPI_Win_create(request->buffer, request->buffer_size, sizeof(char), MPI_INFO_ENV, comm, &request->window);
    request->partition_size = count;
    request->aggregation_factor = 4;
    request->aggregation_factor = (request->aggregation_factor * count > partitions) ? 1 : request->aggregation_factor;
    tree_create(&request->ready_intervals);
    return 0;
}; 

int custom_MPI_Psend_init(const void *buf, int partitions, MPI_Count count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Info info, custom_MPI_Request *request) {
    request->buffer = (char*)buf;
    request->buffer_size = count * partitions;
    request->destination = dest;
    MPI_Win_create(request->buffer, request->buffer_size, sizeof(char), MPI_INFO_ENV, comm, &request->window);
    request->partition_size = count;
    request->aggregation_factor = 4;
    request->aggregation_factor = (request->aggregation_factor * count > partitions) ? 1 : request->aggregation_factor;
    tree_create(&request->ready_intervals);
    return 0;
};

int custom_MPI_Parrived(custom_MPI_Request* request, int partition, int *flag) { 
    // TODO
    return true;
};

int custom_MPI_Pready(int partition, custom_MPI_Request* request) {
    marks_insert(request->ready_intervals, partition, partition + 1);

    int min, max;
    find_interval(request->ready_intervals, request->aggregation_factor, &min, &max);

    // if possible, send aggregated partitions already
    if (max - min >= request->aggregation_factor)
    {
        max = min + request->aggregation_factor;
        MPI_Win_lock(MPI_LOCK_SHARED, request->destination, MPI_MODE_NOCHECK, request->window);
        MPI_Put(&request->buffer[request->partition_size * min], request->partition_size * request->aggregation_factor, MPI_CHAR, request->destination, request->partition_size * min, request->partition_size * request->aggregation_factor, MPI_CHAR, request->window);
        MPI_Win_unlock(request->destination, request->window);
        marks_remove(request->ready_intervals, min, max);
    }
    return 0;
};



