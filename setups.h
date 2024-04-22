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
    MPI_Count buffer_size;
    int iterations;
    int num_send_patterns;
    bool enable_mode[ModeCount];
    int min_thread_count_log[ModeCount];
    int max_thread_count_log[ModeCount];
    int min_partition_size_log[ModeCount];
    int max_partition_size_log[ModeCount];                                                                                         
    SendPattern send_patterns[SendPatternCount]; 
};
typedef struct setup_t* setup;

extern setup select_setup(struct gengetopt_args_info* args);


extern int config_num_test_cases(setup config, Mode mode);

extern int config_max_partition_size(setup config, Mode mode);
extern int config_min_partition_size(setup config, Mode mode);

extern int config_max_thread_count(setup config, Mode mode);
extern int config_min_thread_count(setup config, Mode mode);

extern int config_max_partition_size_total(setup config);
extern int config_min_partition_size_total(setup config);

extern int config_min_partition_size_log_total(setup config);
extern int config_max_partition_size_log_total(setup config);
