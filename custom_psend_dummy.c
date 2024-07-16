#include "custom_psend.h"

#include <mpi.h>

struct custom_request {
    MPI_Request req;
};

int custom_MPI_Start(custom_MPI_Request request) {
    return MPI_Start(&request.req);
}
int custom_MPI_Wait(custom_MPI_Request request, MPI_Status* status) {
    return MPI_Wait(&request.req, status);
}
int custom_MPI_Free(custom_MPI_Request* request) {
    return MPI_Request_free(&request->req);
}

int custom_MPI_Precv_init(void *buf, int partitions, MPI_Count count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Info info, custom_MPI_Request* request) {
    return MPI_Precv_init(buf, partitions, count, datatype, source, tag, comm, info, &request->req);
}
int custom_MPI_Psend_init(const void *buf, int partitions, MPI_Count count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Info info, custom_MPI_Request* request) {
    return MPI_Psend_init(buf, partitions, count, datatype, dest, tag, comm, info, &request->req);
}
int custom_MPI_Parrived(custom_MPI_Request request, int partition, int *flag) {
    return MPI_Parrived(request.req, partition, flag);
}
int custom_MPI_Pready(int partition, custom_MPI_Request request) {
    return MPI_Pready(partition, request.req);
}
int custom_MPI_Pready_list(int length, int array_of_partitions[], custom_MPI_Request request) {
    return MPI_Pready_list(length, array_of_partitions, request.req);
}