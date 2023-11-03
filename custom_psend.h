#include <mpi.h>
#include <pthread.h>
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
typedef struct custom_MPI_Request custom_MPI_Request;

extern int custom_MPI_Start(custom_MPI_Request* request);
extern int custom_MPI_Wait(custom_MPI_Request* request, MPI_Status* status);
extern int custom_MPI_Free(custom_MPI_Request* request);

extern int custom_MPI_Precv_init(void *buf, int partitions, MPI_Count count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Info info, custom_MPI_Request *request);
extern int custom_MPI_Psend_init(const void *buf, int partitions, MPI_Count count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Info info, custom_MPI_Request *request);
extern int custom_MPI_Parrived(custom_MPI_Request* request, int partition, int *flag);
extern int custom_MPI_Pready(int partition, custom_MPI_Request* request);
extern int custom_MPI_Pready_list(int length, const int array_of_partitions[], custom_MPI_Request* request);



