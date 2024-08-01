#include "custom_psend.h"

#include <stdlib.h>
#include <mpi.h>
#include <assert.h>
#include <string.h>
#include <stdatomic.h>

#include <stdio.h>

struct custom_request {
   MPI_Request internal_req;

    atomic_int* public_parts_ready;

    int partition_count;
    int partition_size;

    int internal_partition_count;
    int internal_partition_size;

    // parameters for message aggregation
    int aggregation_count;      // how many partitions may be aggregated
};

int internal_partition(struct custom_request* req, int public_part) {
    return public_part / req->aggregation_count;
}

int first_public_partition(struct custom_request* req, int internal_part) {
    return internal_part * req->aggregation_count;
}

// constants
#define MAX_PARTITION_COUNT_SEND 128
#define MAX_PARTITION_COUNT_RECV 128

int custom_MPI_Precv_init(void *buf, int partitions, MPI_Count count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Info info, custom_MPI_Request* request) {
    custom_MPI_Request r = malloc(sizeof(struct custom_request));
    *request = r;

    r->partition_count = partitions;
    r->partition_size = count;

    r->aggregation_count = r->partition_count / MAX_PARTITION_COUNT_RECV;
    if (r->aggregation_count == 0)
        r->aggregation_count = 1;

    r->internal_partition_count = internal_partition(r, partitions);
    r->internal_partition_size = count * r->aggregation_count;

    assert(r->internal_partition_count * r->aggregation_count == partitions);
    MPI_Precv_init(buf, r->internal_partition_count, r->internal_partition_size, datatype, source, tag, comm, info, &r->internal_req);

    r->public_parts_ready = NULL;

    // printf("mapped %i partitions of size %i to %i internal partitions of size %i\n", r->partition_count, r->partition_size, internal_partition(r, r->partition_count), r->partition_size * r->aggregation_count);

    return 0;
}

int custom_MPI_Psend_init(const void *buf, int partitions, MPI_Count count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Info info, custom_MPI_Request* request) {
    custom_MPI_Request r = malloc(sizeof(struct custom_request));
    *request = r;

    r->partition_count = partitions;
    r->partition_size = count;

    r->aggregation_count = r->partition_count / MAX_PARTITION_COUNT_SEND;
    if (r->aggregation_count == 0)
        r->aggregation_count = 1;

    r->internal_partition_count = internal_partition(r, partitions);
    r->internal_partition_size = count * r->aggregation_count;

    assert(r->internal_partition_count * r->aggregation_count == partitions);
    MPI_Psend_init(buf, r->internal_partition_count, r->internal_partition_size, datatype, dest, tag, comm, info, &r->internal_req);

    r->public_parts_ready = calloc(internal_partition(r, r->partition_count), sizeof(atomic_int));

    return 0;
}

int custom_MPI_Free(custom_MPI_Request* request) {
    MPI_Request_free(&(*request)->internal_req);

    if ((*request)->public_parts_ready)
        free((*request)->public_parts_ready);

    free(*request);
    *request = NULL;
    return 0;
}

int custom_MPI_Start(custom_MPI_Request request) {
    if (request->public_parts_ready) {
        for(int i = 0; i < internal_partition(request, request->partition_count); i++) {
            request->public_parts_ready[i] = ATOMIC_VAR_INIT(0);
        }
    }
    // memset(request->public_parts_ready, 0, sizeof(int) * internal_partition(request, request->partition_count));

    MPI_Start(&request->internal_req);
    return 0;
}

int custom_MPI_Wait(custom_MPI_Request request, MPI_Status* status) {
    return MPI_Wait(&request->internal_req, status);
}


int custom_MPI_Parrived(custom_MPI_Request request, int partition, int *flag) {
    return MPI_Parrived(request->internal_req, internal_partition(request, partition), flag);
}

// mark partition as ready and optionally trigger progress
int custom_MPI_Pready(int partition, custom_MPI_Request request) {
    int internal = internal_partition(request, partition);
    // printf("Mapped Pready(%i) to internal Pready(%i)\n", partition, internal);
    int count = atomic_fetch_add(&request->public_parts_ready[internal], 1);

    if (count == request->aggregation_count - 1) {
        // printf("internal partition %i ready\n", internal);
        return MPI_Pready(internal, request->internal_req);
    }
    
    assert (count <= request->aggregation_count);
    return 0;
}

// mark partitions as ready and optionally trigger progress
int custom_MPI_Pready_list(int length, int array_of_partitions[], custom_MPI_Request request) {
    for (int i = 0; i < length; i++) {
        custom_MPI_Pready(array_of_partitions[i], request);
    }
    return 0;
}

