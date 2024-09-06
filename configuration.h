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


struct configuration_t {
    const char* name;
    MPI_Count buffer_size;

    // warmup
    int warmup_iterations;
    int post_warmup_sleep;

    int iterations;

    // which transfer mechanisms to benchmark
    bool enable_mode[ModeCount];

    // whether partition sizes may differ between sender and receiver
    bool different_partition_sizes[ModeCount];

    // logarithms of thread counts to run
    int min_thread_count_log[ModeCount];
    int max_thread_count_log[ModeCount];
    int min_partition_size_log[ModeCount];
    int max_partition_size_log[ModeCount];

    // list of send patterns
    int num_send_patterns;
    SendPattern send_patterns[SendPatternCount]; 
};
typedef struct configuration_t* configuration;


configuration config_from_args(struct gengetopt_args_info* args);

const char* config_name(configuration config);

MPI_Count config_buffer_size(configuration config);
int config_iterations(configuration config);

int config_num_test_cases(configuration config, Mode mode);

int config_allow_different_partition_sizes(configuration config, Mode mode);

int config_max_partition_size(configuration config, Mode mode);
int config_min_partition_size(configuration config, Mode mode);

int config_max_thread_count(configuration config, Mode mode);
int config_min_thread_count(configuration config, Mode mode);

int config_max_partition_size_total(configuration config);
int config_min_partition_size_total(configuration config);

int config_min_partition_size_log_total(configuration config);
int config_max_partition_size_log_total(configuration config);
