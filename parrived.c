#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>

void bench_psend_parrived(int comm_rank, void* buffer, long partition_count, long partition_count_recv, long partition_size, long partition_size_recv, int iteration_count)
{
    // init requests and timer
    MPI_Request request;

    if (comm_rank == 0)
    {
        MPI_Psend_init(buffer, partition_count, partition_size, MPI_CHAR, 1, 0, MPI_COMM_WORLD, MPI_INFO_ENV, &request);
    }
    else if (comm_rank == 1)
    {
        MPI_Precv_init(buffer, partition_count_recv, partition_size_recv, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_INFO_ENV, &request);
    }

    // run
    MPI_Barrier(MPI_COMM_WORLD);

    if (comm_rank == 0)
    {
        for (size_t i = 0; i < iteration_count; i++)
        {
            // printf("Send rank starting\n");
            MPI_Start(&request);

            for (size_t p = 0; p < partition_count; p++)
            {
                unsigned int partition_num = p;
                MPI_Pready(partition_num, request);
            }

            // printf("Send rank waiting\n");
            MPI_Wait(&request, MPI_STATUS_IGNORE);
        }
    }
    else if (comm_rank == 1)
    {
        for (size_t i = 0; i < iteration_count; i++)
        {
            // printf("Receive rank starting\n");
            MPI_Start(&request);

            int flag = 0;
            while (!flag)
            {
                flag = 1;
                for (size_t p = 0; p < partition_count_recv; p++)
                {
                    int _flag = 0;
                    // printf("completion: "); fflush(stdout);
                    MPI_Request_get_status(request, &_flag, MPI_STATUS_IGNORE);
                    flag = flag && _flag;
                    // printf("%i, Parrived: ", _flag); fflush(stdout);

                    // sometimes deadlocks when Parrived is called and request is already completed (even if get_status is not called) 
                    // enable this to prevent unexpected behaviour
                    if (flag)
                        break;

                    MPI_Parrived(request, p, &_flag);
                    // printf(" %i\n", _flag);
                    flag = flag && _flag;
                }
            }

            printf("Receive rank waiting\n"); fflush(stdout);
            MPI_Wait(&request, MPI_STATUS_IGNORE);
            printf("done\n"); fflush(stdout);
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Request_free(&request);
    printf("Done\n");
};


int main(int argc, char* argv[]) {
    int thread_support;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &thread_support);
    if (thread_support != MPI_THREAD_MULTIPLE)
        return -1;

    int comm_rank, comm_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &comm_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);

    int buffer_size = 4 * 1024 * 1024;
    void* buffer = malloc(sizeof(char) * buffer_size);

    int partition_count      = 8;
    int partition_count_recv = 16;
    int partition_size       = buffer_size / partition_count;
    int partition_size_recv  = buffer_size / partition_count_recv;

    bench_psend_parrived(comm_rank, buffer, partition_count, partition_count_recv, partition_size, partition_size_recv, 100);

    MPI_Finalize();
}
