#include <mpi.h>



struct custom_request;
typedef struct custom_request* custom_MPI_Request;


extern int custom_MPI_Precv_init(void *buf, int partitions, MPI_Count count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Info info, custom_MPI_Request* request);
extern int custom_MPI_Psend_init(const void *buf, int partitions, MPI_Count count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Info info, custom_MPI_Request* request);
extern int custom_MPI_Free(custom_MPI_Request* request);


extern int custom_MPI_Start(custom_MPI_Request request);
extern int custom_MPI_Wait(custom_MPI_Request request, MPI_Status* status);

extern int custom_MPI_Parrived(custom_MPI_Request request, int partition, int *flag);
extern int custom_MPI_Pready(int partition, custom_MPI_Request request);
extern int custom_MPI_Pready_list(int length, int array_of_partitions[], custom_MPI_Request request);

extern int custom_MPI_Request_get_status(custom_MPI_Request request, int* flag, MPI_Status *status);
