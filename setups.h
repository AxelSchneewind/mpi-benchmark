#pragma once

#include "bench.h"

#include "cmdline.h"

#include <mpi.h>
#include <memory.h>
#include <stdbool.h>

// powers of two for buffer sizes
#define B (MPI_Count)1
#define KB (MPI_Count)(1024)
#define MB (MPI_Count)(1024 * 1024)
#define GB (MPI_Count)(1024 * 1024 * 1024)


struct setup_t {
    int num_test_cases;
    MPI_Count buffer_size;
    int iterations;
    int computations_per_element;
    int mode;
    int min_thread_count;
    int max_thread_count;
    bool allow_1_to_n;
    bool allow_n_to_1;
    int min_partition_size;
    int max_partition_size;                                                                                         
    // TODO: only accept one send pattern
    int num_send_patterns;
    SendPattern send_patterns[SendPatternCount]; 
};
typedef struct setup_t* setup;

extern setup make_setup(struct gengetopt_args_info* args_info);
