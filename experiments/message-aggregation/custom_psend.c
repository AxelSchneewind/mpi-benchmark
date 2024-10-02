#include "custom_psend.h"
#include "intervals.h"

#include <stdlib.h>
#include <mpi.h>
#include <assert.h>

struct custom_request {
    // 
    char is_sender;

    // parameters from Psend initialization
    const char* buffer;
    MPI_Count buffer_size;

    int partition_count;
    int partition_size;

    MPI_Datatype datatype;
    int other_rank;
    int tag; 

    MPI_Comm comm; 
    MPI_Info info; 

    // window for RMA transfers
    MPI_Win window;

    // data structure for the marked partitions, allowing the extraction of intervals
    struct interval_list ready_list;

    // parameters for message aggregation
    int aggregation_count;      // how many partitions may be aggregated
    int progress_counter;       // counter for the number of calls to Pready
};

// transfer an interval of partitions via RMA
void send_interval(struct custom_request* info, struct interval partitions) {
    assert(partitions.right > partitions.left);
    assert(partitions.right >= 0);
    assert(partitions.left  >= 0);

    MPI_Win_lock(MPI_LOCK_SHARED, info->other_rank, MPI_MODE_NOCHECK, info->window);

    int displacement = (info->partition_size * partitions.left);
    int count = info->partition_size * (partitions.right - partitions.left);
    MPI_Put(info->buffer + displacement
          , count
          , info->datatype
          , info->other_rank
          , displacement
          , count
          , info->datatype
          , info->window);

    MPI_Win_unlock(info->other_rank, info->window);
}

// get largest interval if present and optionally send it
int try_progress(struct custom_request* info) {
    struct interval partitions = interval_list_pull_largest(&info->ready_list);

    if (!interval_is_empty(&partitions)) {
        send_interval(info, partitions);
        return 0;
    }
    return 1;
};

// get largest interval if present and send it
int progress(struct custom_request* info) {
    struct interval partitions = interval_list_pull_largest(&info->ready_list/*, info->aggregation_count*/);

    if (!interval_is_empty(&partitions)) {
        send_interval(info, partitions);
        return 0;
    }
    return 1;
};


int custom_MPI_Precv_init(void *buf, int partitions, MPI_Count count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Info info, custom_MPI_Request* request) {
    custom_MPI_Request r = malloc(sizeof(struct custom_request));
    *request = r;

    r->is_sender = 0;
    r->buffer = buf;
    r->partition_count = partitions;
    r->buffer_size = partitions * count;
    r->partition_size = count;
    r->datatype = datatype; 
    r->other_rank = source; 
    r->tag = tag;
    r->comm = comm;
    r->info = info;
    r->aggregation_count = 0;
    r->progress_counter = 0;
    interval_list_init(&r->ready_list, 0);

	MPI_Win_create(r->buffer, r->buffer_size, 1, r->info, r->comm, &r->window);
    return 0;
}

int custom_MPI_Psend_init(const void *buf, int partitions, MPI_Count count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Info info, custom_MPI_Request* request) {
    custom_MPI_Request r = malloc(sizeof(struct custom_request));
    *request = r;

    r->is_sender = 1;
    r->buffer = buf;
    r->partition_count = partitions;
    r->buffer_size = partitions * count;
    r->partition_size = count;
    r->datatype = datatype; 
    r->other_rank = dest; 
    r->tag = tag;
    r->comm = comm;
    r->info = info;

    r->progress_counter = 0;
    r->aggregation_count = 4;
    if (r->aggregation_count > partitions)
        r->aggregation_count = partitions;
    interval_list_init(&r->ready_list, r->partition_count);

	MPI_Win_create(r->buffer, r->buffer_size, 1, r->info, r->comm, &r->window);
    return 0;
}

int custom_MPI_Free(custom_MPI_Request* request) {
    MPI_Win_free(&(*request)->window);
    interval_list_free(&(*request)->ready_list);
    free(*request);
    *request = NULL;
    return 0;
}

int custom_MPI_Start(custom_MPI_Request request) {
    interval_list_reset(&request->ready_list);
    return 0;
}
int custom_MPI_Wait(custom_MPI_Request request, MPI_Status* status) {
    if (request->is_sender) {
        int ret = 0;
        while (0 == ret) {
            ret = progress(request);
        }
    //     MPI_Barrier(request->comm);
    // } else {
    //     MPI_Barrier(request->comm);
    }
    return MPI_Win_fence(0, request->window);
}


// partial completion testing is not supported
int custom_MPI_Parrived(custom_MPI_Request request, int partition, int *flag) {
    *flag = 0;
    return 0;
}

// mark partition as ready and optionally trigger progress
int custom_MPI_Pready(int partition, custom_MPI_Request request) {
    interval_list_insert(&request->ready_list, partition);
    if ((++request->progress_counter) % request->aggregation_count == 0)
        try_progress(request);
    return 0;
}

// mark partitions as ready and optionally trigger progress
int custom_MPI_Pready_list(int length, int array_of_partitions[], custom_MPI_Request request) {
    interval_list_insert_list(&request->ready_list, array_of_partitions, length);
    if ((request->progress_counter += length) % request->aggregation_count == 0)
        try_progress(request);
    return 0;
}