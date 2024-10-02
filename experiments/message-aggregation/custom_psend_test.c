
#include "custom_psend.h"

#include "send_patterns.h"

#include <stdlib.h>
#include <mpi.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>




void run_custom_psend(int comm_rank, char* buffer, int partition_count, int partition_count_recv, int partition_size, int partition_size_recv, int* send_pattern)
{
    int buffer_size = partition_count * partition_size;

	// init
	custom_MPI_Request request;
    MPI_Status status;

	if (comm_rank == 0) {
		custom_MPI_Psend_init(buffer, partition_count, partition_size, MPI_CHAR, 1, 0, MPI_COMM_WORLD, MPI_INFO_ENV, &request);
	} else if (comm_rank == 1) {
		custom_MPI_Precv_init(buffer, partition_count_recv, partition_size_recv, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_INFO_ENV, &request);
	}

	// run
	MPI_Barrier(MPI_COMM_WORLD);

	for (size_t i = 0; i < 1000; i++) {
		if (comm_rank == 0) {
			custom_MPI_Start(request);

			for (size_t p = 0; p < partition_count; p++) {
				unsigned int partition_num = send_pattern[p];
				custom_MPI_Pready(partition_num, request);
			}

			custom_MPI_Wait(request, &status);
		} else if (comm_rank == 1) {
			custom_MPI_Start(request);
			custom_MPI_Wait(request, &status);
		}
	}

	MPI_Barrier(MPI_COMM_WORLD);

	custom_MPI_Free(&request);
};


int main (int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int comm_rank, comm_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &comm_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);

    int buffer_size = 512; // 4MB
    void* buffer = malloc(sizeof(char) * buffer_size + 1);
    ((char*)buffer)[buffer_size] = 0;

    char content[] = "# This is a string.                                            #";
    int content_len = (sizeof(content) - 1) / sizeof(content[0]);

    // original buffer (data that should be receveived)
    void* buffer_original = malloc(sizeof(char) * buffer_size + 1);
    for (int i = 0; i < buffer_size / (sizeof(content) - 1) * sizeof(content[0]); i++) {
        memcpy(&((char*)buffer_original)[i * content_len], content, content_len);
    }

    if (0 == comm_rank) {
        strcpy(buffer, buffer_original);
    } else {
        memset(buffer, 0, buffer_size);
    }

    // output current buffers
    if (0 == comm_rank) {
        printf("Initial sender buffer:\n%s\n", buffer);
        fflush(stdout);
    } else {
        printf("Initial receiver buffer:\n%s\n", buffer);
        fflush(stdout);
    }

    int partition_size       = 32;//768;
    int partition_size_recv  = 32;//768;
    int partition_count      = buffer_size / partition_size;
    int partition_count_recv = buffer_size / partition_size_recv;

    permutation pattern;
    permutation_create(&pattern, partition_count);
    make_random_pattern(pattern, partition_count);

    if (0 == comm_rank) 
        printf("sending %i bytes split into %i partitions of %i bytes each\n", buffer_size, partition_count, partition_size);
    run_custom_psend(comm_rank, buffer, partition_count, partition_count_recv, partition_size, partition_size_recv, pattern);


    // output current buffers
    if (0 == comm_rank) {
        // printf("Final sender buffer:\n%s\n", buffer);
        // fflush(stdout);
    } else {
        printf("Final receiver buffer:\n%s\n", buffer);
        fflush(stdout);
    }

    if (1 == comm_rank)
        printf("strcmp of buffers: %i\n", strcmp(buffer, buffer_original));

    MPI_Finalize();
}