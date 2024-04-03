#include "setups.h"

#include <math.h>

static const char* setup_names[] = {
    "OPENMPI_HAWK",
    "OPENMPI_LOCAL",
    "FULL_HAWK",
    "FULL_LOCAL",
    "RDMA_LOCAL",
    "PARTITIONED_LOCAL",
    "CUSTOM_LOCAL"
};

#define DEFAULT_BUFFER_SIZE (MPI_Count)(8 * MB)


struct setup_t setups[] = {
    // openmpi
    {
        .buffer_size = 8 * MB,
        .iterations = 100,
        //                              Send = 0, SendPersistent = 1, Isend = 2, IsendTest = 4, IsendThenTest = 5, IsendTestall = 6, CustomPsend = 7, WinSingle = 8,            Win = 9,   Psend = 10, PsendList = 11, PsendParrived = 12, PsendProgress = 13, PsendProgressThreaded = 14
        .enable_mode =            {         true,               true,      true,          true,              true,             true,           false,          true,               true,         true,           true,               true,              false,                      false},
        .min_partition_size_log = {            8,                  8,         8,             8,                 8,                8,               8,            10,             23 - 7,            8,              8,                  8,                  8,                          8},
        .max_partition_size_log = {           23,                 23,        23,            23,                23,               23,              23,            23,                 23,           23,             23,                 23,                 23,                         23},
        .min_thread_count_log=    {            0,                  0,         0,             0,                 0,                0,               0,             0,                  0,            0,              0,                  0,                  0,                          0},
        .max_thread_count_log=    {            6,                  6,         6,             6,                 6,                6,               6,             6,                  6,            6,              6,                  6,                  6,                          6},
        .send_patterns =          { Linear, Stride16K, Random, RandomBurst1K },
        .num_send_patterns =      4
    },
    // openmpi local
    {
        .buffer_size = 8 * MB,
        .iterations = 10,
        //                              Send = 0, SendPersistent = 1, Isend = 2, IsendTest = 4, IsendThenTest = 5, IsendTestall = 6, CustomPsend = 7, WinSingle = 8,            Win = 9,   Psend = 10, PsendList = 11, PsendParrived = 12, PsendProgress = 13, PsendProgressThreaded = 14
        .enable_mode =            {         true,               true,      true,          true,              true,             true,           false,          true,               true,         true,           true,               true,              false,                      false},
        .min_partition_size_log = {           10,                 10,        10,            10,                10,               10,              10,            10,            23 - 10,           10,             10,                 10,                 10,                         10},
        .max_partition_size_log = {           23,                 23,        23,            23,                23,               23,              23,            23,                 23,           23,             23,                 23,                 23,                         23},
        .min_thread_count_log=    {            0,                  0,         0,             0,                 0,                0,               0,             0,                  0,            0,              0,                  0,                  0,                          0},
        .max_thread_count_log=    {            6,                  6,         6,             6,                 6,                6,               6,             6,                  6,            6,              6,                  6,                  6,                          6},
        .send_patterns =          { Linear, Stride16K, Random, RandomBurst1K },
        .num_send_patterns =  4
    },
    // full bench on hawk
    {
        .buffer_size = 8 * MB,
        .iterations = 100,
        //                              Send = 0, SendPersistent = 1, Isend = 2, IsendTest = 4, IsendThenTest = 5, IsendTestall = 6, CustomPsend = 7, WinSingle = 8,            Win = 9,   Psend = 10, PsendList = 11, PsendParrived = 12, PsendProgress = 13, PsendProgressThreaded = 14
        .enable_mode =            {         true,               true,      true,          true,              true,             true,           false,          true,               true,         true,           true,               true,               true,                       true},
        .min_partition_size_log = {            9,                  9,         9,             9,                 9,                9,               9,            10,            23 - 10,            9,             10,                  9,                  9,                          9},
        .max_partition_size_log = {           23,                 23,        23,            23,                23,               23,              23,            23,                 23,           23,             23,                 23,                 23,                         23},
        .min_thread_count_log=    {            0,                  0,         0,             0,                 0,                0,               0,             0,                  0,            0,              0,                  0,                  0,                          0},
        .max_thread_count_log=    {            6,                  6,         6,             6,                 6,                6,               6,             6,                  6,            6,              6,                  6,                  6,                          6},
        .send_patterns =          { Linear, Stride16K, Random, RandomBurst1K },
        .num_send_patterns =  4
    },
    // full bench locally
    {
        .buffer_size = 8 * MB,
        .iterations = 30,
        //                              Send = 0, SendPersistent = 1, Isend = 2, IsendTest = 4, IsendThenTest = 5, IsendTestall = 6, CustomPsend = 7, WinSingle = 8,            Win = 9,   Psend = 10, PsendList = 11, PsendParrived = 12, PsendProgress = 13, PsendProgressThreaded = 14
        .enable_mode =            {         true,               true,      true,         false,             false,            false,           false,          true,               true,         true,            true,               true,              false,                       true},
        .min_partition_size_log = {            9,                  9,         9,            10,                10,               10,              10,            10,            23 - 10,           10,              10,                 10,                 10,                         10},
        .max_partition_size_log = {           23,                 23,        23,            23,                23,               23,              23,            23,                 23,           23,              23,                 23,                 23,                         23},
        .min_thread_count_log=    {            0,                  0,         0,             0,                 0,                0,               0,             0,                  0,            0,               0,                  0,                  0,                          0},
        .max_thread_count_log=    {            6,                  6,         6,             6,                 6,                6,               6,             6,                  6,            6,               6,                  6,                  6,                          6},
        .send_patterns =          { Linear, Stride16K, Random, RandomBurst1K },
        .num_send_patterns =  4
    },
    // rdma local
    {
        .buffer_size = 8 * MB,
        .iterations = 10,
        //                              Send = 0, SendPersistent = 1, Isend = 2, IsendTest = 4, IsendThenTest = 5, IsendTestall = 6, CustomPsend = 7, WinSingle = 8,            Win = 9,   Psend = 10, PsendList = 11, PsendParrived = 12, PsendProgress = 13, PsendProgressThreaded = 14
        .enable_mode =            {         true,              false,      true,         false,             false,            false,           false,          true,               true,        false,          false,              false,              false,                      false},
        .min_partition_size_log = {            9,                  9,         9,            10,                10,               10,              10,            10,            23 - 10,           10,             10,                 10,                 10,                         10},
        .max_partition_size_log = {           23,                 23,        23,            23,                23,               23,              23,            23,                 23,           23,             23,                 23,                 23,                         23},
        .min_thread_count_log=    {            0,                  0,         0,             0,                 0,                0,               0,             0,                  0,            0,              0,                  0,                  0,                          0},
        .max_thread_count_log=    {            6,                  6,         6,             6,                 6,                6,               6,             6,                  6,            6,              6,                  6,                  6,                          6},
        .send_patterns =          { Linear, Stride16K, Random, RandomBurst1K },
        .num_send_patterns =  4
    },
    // partitioned local
    {
        .buffer_size = 8 * MB,
        .iterations = 10,
        //                              Send = 0, SendPersistent = 1, Isend = 2, IsendTest = 4, IsendThenTest = 5, IsendTestall = 6, CustomPsend = 7, WinSingle = 8,            Win = 9,   Psend = 10, PsendList = 11, PsendParrived = 12, PsendProgress = 13, PsendProgressThreaded = 14
        .enable_mode =            {         true,              false,      true,         false,             false,            false,           false,         false,              false,         true,           true,               true,               true,                       true},
        .min_partition_size_log = {            9,                  9,         9,            10,                10,               10,              10,            10,            23 - 10,           10,             10,                 10,                 10,                         10},
        .max_partition_size_log = {           23,                 23,        23,            23,                23,               23,              23,            23,                 23,           23,             23,                 23,                 23,                         23},
        .min_thread_count_log=    {            0,                  0,         0,             0,                 0,                0,               0,             0,                  0,            0,              0,                  0,                  0,                          0},
        .max_thread_count_log=    {            6,                  6,         6,             6,                 6,                6,               6,             6,                  6,            6,              6,                  6,                  6,                          6},
        .send_patterns =          { Linear, Stride16K, Random, RandomBurst1K },
        .num_send_patterns =  4
    },
    // custom local
    {
        .buffer_size = 8 * MB,
        .iterations = 10,
        //                              Send = 0, SendPersistent = 1, Isend = 2, IsendTest = 4, IsendThenTest = 5, IsendTestall = 6, CustomPsend = 7, WinSingle = 8,            Win = 9,   Psend = 10, PsendList = 11, PsendParrived = 12, PsendProgress = 13, PsendProgressThreaded = 14
        .enable_mode =            {         true,               true,      true,         false,             false,            false,            true,         false,              false,        false,           true,              false,              false,                      false},
        .min_partition_size_log = {            9,                  9,         9,            10,                10,               10,              10,            10,            23 - 10,           10,             10,                 10,                 10,                         10},
        .max_partition_size_log = {           23,                 23,        23,            23,                23,               23,              23,            23,                 23,           23,             23,                 23,                 23,                         23},
        .min_thread_count_log =   {            0,                  0,         0,             0,                 0,                0,               0,             0,                  0,            0,              0,                  0,                  0,                          0},
        .max_thread_count_log =   {            6,                  6,         6,             6,                 6,                6,               6,             6,                  6,            6,              6,                  6,                  6,                          6},
        .send_patterns =          { Linear, Stride16K, Random, RandomBurst1K },
        .num_send_patterns =  4
    }
};

setup select_setup(const char* name) {
    for (int i = 0; i < sizeof(setups)/ sizeof(setups[0]); i++)
    {
        if (0 == strcasecmp(name, setup_names[i])) {
            return &setups[i];
        }
    }
    return NULL;
}

int num_test_cases(setup config, Mode mode) {
#ifdef DISABLE_PSEND
        if (is_psend(mode))
            return 0;
#endif

    return (config->enable_mode[mode])
     * (config->num_send_patterns)
      * (is_psend(mode) ? (config->max_partition_size_log - config->min_partition_size_log + 1) : 1)
       * (config->max_partition_size_log - config->min_partition_size_log + 1)
        * (config->max_thread_count_log - config->min_thread_count_log + 1);
}

int setup_min_partition_size_log_total(setup config) {
    int result = config->min_partition_size_log[0];
    for (int i = 0; i < ModeCount; ++i) {
        if (config->min_partition_size_log[i] < result) {
            result = config->min_partition_size_log[i];
        }
    }
    return result;
}

int setup_max_partition_size_log_total(setup config) {
    int result = config->max_partition_size_log[0];
    for (int i = 0; i < ModeCount; ++i) {
        if (config->max_partition_size_log[i] > result) {
            result = config->max_partition_size_log[i];
        }
    }
    return result;
}

int setup_min_partition_size_total(setup config) {
    return (1 << setup_min_partition_size_log_total(config));
}

int setup_max_partition_size_total(setup config) {
    return (1 << setup_max_partition_size_log_total(config));
}

int setup_max_partition_size(setup config, Mode mode) {
    return (1 << config->max_partition_size_log[mode]);
} 

int setup_min_partition_size(setup config, Mode mode) {
    return (1 << config->min_partition_size_log[mode]);
}

int setup_max_thread_count(setup config, Mode mode){
    return (1 << config->max_thread_count_log[mode]);
}
int setup_min_thread_count(setup config, Mode mode){
    return (1 << config->min_thread_count_log[mode]);
}