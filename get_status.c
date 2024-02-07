#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>

void bench_psend_progress(int comm_rank, void* buffer, long partition_count, long partition_count_recv, long partition_size, long partition_size_recv, int iteration_count)
{
    // init request 
    MPI_Request request;

    // printf("MPI_Psendrecv_init()\n");
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
            // printf("MPI_Start()\n"); fflush(stdout);
            MPI_Start(&request);

            int flag = false;
            for (size_t p = 0; p < partition_count; p++)
            {
                unsigned int partition_num = p;

                // sometimes gets stuck here, works if MPI_Request_get_status() is not called here
                // printf("MPI_Request_get_status()\n"); fflush(stdout);
                MPI_Request_get_status(request, &flag, MPI_STATUS_IGNORE);

                // if (true == flag) {   // MPI_Request_get_status returns true before all Pready calls have been made?
                //               // does not occur with mpich and only if i % 2 == 1
                //     printf("How did I get here (partition %i of %i, iteration %i)\n", p, partition_count, i); fflush(stdout);
                // }

                // printf("MPI_Pready()\n"); fflush(stdout);
                MPI_Pready(partition_num, request);
            }

            // printf("MPI_Wait()\n"); fflush(stdout);

            // sometimes gets stuck here
            MPI_Wait(&request, MPI_STATUS_IGNORE);
        }
    } else if (comm_rank == 1) {
        for (size_t i = 0; i < iteration_count; i++)
        {
            MPI_Start(&request);

            // 
            int flag = false;
            while(!flag)
            {
                MPI_Request_get_status(request, &flag, MPI_STATUS_IGNORE);
            }

            MPI_Wait(&request, MPI_STATUS_IGNORE);
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);

    MPI_Request_free(&request);
    // printf("done\n");
};



int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int comm_rank, comm_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &comm_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);

    int buffer_size = 4 * 1024 * 1024; // 4MB
    void* buffer = malloc(sizeof(char) * buffer_size);

    int partition_size       = 4194304;//32768;
    int partition_size_recv  = 4194304;
    int partition_count      = buffer_size / partition_size;
    int partition_count_recv = buffer_size / partition_size_recv;//262144;

    for (int i = 0; i < 1000; i++)  {
        fprintf("\rrun %i", i);
        bench_psend_progress(comm_rank, buffer, partition_count, partition_count_recv, partition_size, partition_size_recv, 100);
    }

    MPI_Finalize();
}
