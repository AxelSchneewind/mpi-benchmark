#include <mpi.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdbool.h>

#include "interval_tree.h"

struct custom_MPI_Request_t
{
    int destination;
    bool is_sender;

    int partition_size;
    int aggregation_factor;

    char* buffer;
    MPI_Count buffer_size;

    MPI_Win window;
    tree ready_intervals;
};
typedef struct custom_MPI_Request_t* custom_MPI_Request;
int custom_MPI_Start(custom_MPI_Request request) {
    if (request->is_sender) {
        MPI_Win_lock(MPI_LOCK_EXCLUSIVE, request->destination, MPI_MODE_NOCHECK, request->window);
    }
    return 0;
};
int custom_MPI_Wait(custom_MPI_Request request, MPI_Status* status) {
    if (request->is_sender) {
        int min, max;
        int size = request->aggregation_factor;
        while (size >= 1) {
            tree_find_interval(request->ready_intervals, size, &min, &max);
            while (max - min >= size) {
                MPI_Put(&request->buffer[request->partition_size * min], request->partition_size * request->aggregation_factor, MPI_CHAR, request->destination, request->partition_size * min, request->partition_size * request->aggregation_factor, MPI_CHAR, request->window);
                tree_remove(request->ready_intervals, min, max);
                tree_find_interval(request->ready_intervals, size, &min, &max);
            }
            size--;
        }

        MPI_Win_unlock(request->destination, request->window);
    } else {
        MPI_Win_lock(MPI_LOCK_EXCLUSIVE, request->destination, MPI_MODE_NOCHECK, request->window);
        MPI_Win_unlock(request->destination, request->window);
    }

    return 0;
};

int custom_MPI_Free(custom_MPI_Request* request) {
    MPI_Win_free(&(*request)->window);
    tree_destroy(&(*request)->ready_intervals);
    free(*request);
    *request = NULL;
    return 0;
};

int custom_MPI_Precv_init(void *buf, int partitions, MPI_Count count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Info info, custom_MPI_Request* request) {
    custom_MPI_Request result = malloc(sizeof(struct custom_MPI_Request_t));
    result->buffer = (char*)buf;
    result->buffer_size = count * partitions;
    result->destination = source;
    result->is_sender = false;
    MPI_Win_create(result->buffer, result->buffer_size, sizeof(char), MPI_INFO_ENV, comm, &result->window);
    result->partition_size = count;
    result->aggregation_factor = 1;
    result->aggregation_factor = (result->aggregation_factor * count > partitions) ? partitions : result->aggregation_factor;
    tree_create(&result->ready_intervals);
    *request = result;
    return 0;
}; 

int custom_MPI_Psend_init(const void *buf, int partitions, MPI_Count count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Info info, custom_MPI_Request *request) {
    custom_MPI_Request result = malloc(sizeof(struct custom_MPI_Request_t));
    result->buffer = (char*)buf;
    result->buffer_size = count * partitions;
    result->destination = dest;
    result->is_sender = true;
    MPI_Win_create(result->buffer, result->buffer_size, sizeof(char), MPI_INFO_ENV, comm, &result->window);
    result->partition_size = count;
    result->aggregation_factor = 1;
    result->aggregation_factor = (result->aggregation_factor * count > partitions) ? partitions : result->aggregation_factor;
    tree_create(&result->ready_intervals);
    *request = result;
    return 0;
};

int custom_MPI_Parrived(custom_MPI_Request request, int partition, int *flag) { 
    // TODO
    *flag = 1;
    return 0;
};

int custom_MPI_Pready(int partition, custom_MPI_Request request) {
    if (request->is_sender) {
        tree_insert(request->ready_intervals, partition, partition + 1);

        int min, max;
        tree_find_interval(request->ready_intervals, request->aggregation_factor, &min, &max);

        // if possible, send aggregated partitions already
        if (max - min >= request->aggregation_factor)
        {
            max = min + request->aggregation_factor;
            MPI_Put(&request->buffer[request->partition_size * min], request->partition_size * request->aggregation_factor, MPI_CHAR, request->destination, request->partition_size * min, request->partition_size * request->aggregation_factor, MPI_CHAR, request->window);
            tree_remove(request->ready_intervals, min, max);
        }
        return 0;
    } else {
        return -1;
    }
};



int custom_MPI_Pready_list(int length, const int array_of_partitions[], custom_MPI_Request request) {
    // not implemented yet
    return -1;
};