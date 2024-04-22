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
        .enable_mode =            {         true,               true,      true,          true,             false,            false,           false,          true,               true,         true,           true,               true,              false,                      false},
        .min_partition_size_log = {            8,                  8,         8,             8,                 8,                8,               8,            10,             23 - 4,            8,              8,                  8,                  8,                          8},
        .max_partition_size_log = {           23,                 23,        23,            23,                23,               23,              23,            23,                 23,           23,             23,                 23,                 23,                         23},
        .min_thread_count_log=    {            0,                  0,         0,             0,                 0,                0,               0,             0,                  0,            0,              0,                  0,                  0,                          0},
        .max_thread_count_log=    {            4,                  6,         6,             6,                 4,                4,               4,             4,                  4,            6,              6,                  0,                  4,                          6},
        .send_patterns =          { Linear, /*Stride16K,*/ Random/*, RandomBurst1K*/ },
        .num_send_patterns =      2
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
        .max_thread_count_log=    {            4,                  4,         4,             4,                 4,                4,               4,             4,                  4,            4,              4,                  0,                  4,                          6},
        .send_patterns =          { Linear, Stride16K, Random, RandomBurst1K },
        .num_send_patterns =  4
    },
    // full bench on hawk
    {
        .buffer_size = 8 * MB,
        .iterations = 100,
        //                              Send = 0, SendPersistent = 1, Isend = 2, IsendTest = 4, IsendThenTest = 5, IsendTestall = 6, CustomPsend = 7, WinSingle = 8,            Win = 9,   Psend = 10, PsendList = 11, PsendParrived = 12, PsendProgress = 13, PsendProgressThreaded = 14
        .enable_mode =            {         true,               true,      true,          true,             false,            false,           false,          true,               true,         true,           true,               true,               true,                       true},
        .min_partition_size_log = {            9,                  9,         9,             9,                 9,                9,               9,            10,            23 - 10,            9,             10,                  9,                  9,                          9},
        .max_partition_size_log = {           23,                 23,        23,            23,                23,               23,              23,            23,                 23,           23,             23,                 23,                 23,                         23},
        .min_thread_count_log=    {            0,                  0,         0,             0,                 0,                0,               0,             0,                  0,            0,              0,                  0,                  0,                          0},
        .max_thread_count_log=    {            4,                  6,         6,             6,                 4,                4,               4,             4,                  4,            6,              6,                  0,                  4,                          6},
        .send_patterns =          { Linear, /*Stride16K,*/ Random/*, RandomBurst1K*/ },
        .num_send_patterns =  2
    },
    // full bench locally
    {
        .buffer_size = 8 * MB,
        .iterations = 30,
        //                              Send = 0, SendPersistent = 1, Isend = 2, IsendTest = 4, IsendThenTest = 5, IsendTestall = 6, CustomPsend = 7, WinSingle = 8,            Win = 9,   Psend = 10, PsendList = 11, PsendParrived = 12, PsendProgress = 13, PsendProgressThreaded = 14
        .enable_mode =            {         true,               true,      true,          true,              true,             true,           false,          true,               true,         true,            true,               true,              false,                       true},
        .min_partition_size_log = {           12,                 12,        12,            12,                12,               12,              12,            12,            23 - 10,           12,              12,                 12,                 12,                         12},
        .max_partition_size_log = {           23,                 23,        23,            23,                23,               23,              23,            23,                 23,           23,              23,                 23,                 23,                         23},
        .min_thread_count_log=    {            0,                  0,         0,             0,                 0,                0,               0,             0,                  0,            0,               0,                  0,                  0,                          0},
        .max_thread_count_log=    {            4,                  4,         4,             4,                 4,                4,               4,             4,                  4,            4,               4,                  0,                  4,                          6},
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
        .max_thread_count_log=    {            4,                  4,         4,             4,                 4,                4,               4,             4,                  4,            4,              4,                  0,                  4,                          6},
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
        .max_thread_count_log=    {            4,                  4,         4,             4,                 4,                4,               4,             4,                  4,            4,              4,                  0,                  4,                          6},
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
        .max_thread_count_log =   {            4,                  4,         4,             4,                 4,                4,               4,             4,                  4,            4,              4,                  0,                  4,                          6},
        .send_patterns =          { Linear, Stride16K, Random, RandomBurst1K },
        .num_send_patterns =  4
    }
};

setup select_setup(struct gengetopt_args_info* args) {
    setup result = malloc(sizeof(struct setup_t));

    const int num_modes = args->modes_given;
    const int num_send_patterns = args->send_patterns_given;
    int mode[ModeCount];

    memset(&result->enable_mode, 0, sizeof(result->enable_mode));
    memset(mode, 0, sizeof(int) * ModeCount);
    result->iterations = args->iteration_count_arg;
    result->buffer_size = (1 << args->buffer_size_arg);
    result->num_send_patterns = (0 == num_send_patterns) ? 1 : num_send_patterns;

    // check which modes are selected
    for (int i = 0; i < num_modes; i++) {
        for (int j = 0; j < ModeCount; j++) {
            if (0 == strcasecmp(args->modes_arg[i], mode_names[j])) {
                mode[i] = j;
                result->enable_mode[j] = true;
            }
        }
    }

    // check number of arguments
    if (num_modes != args->min_partition_size_given
     || num_modes != args->max_partition_size_given
     || num_modes != args->max_thread_count_given
     || num_modes != args->min_thread_count_given) {

        // if only single argument each, use that for all
        if (1 != args->min_partition_size_given
         || 1 != args->max_partition_size_given
         || 1 != args->max_thread_count_given
         || 1 != args->min_thread_count_given) {
            printf("number of arguments for partition sizes or thread counts does not match number of modes selected\n");
            exit(1);
        } else {
            // set min/max partition sizes/thread counts to single argument each
            for (int i = 0; i < num_modes; i++) {
                result->min_partition_size_log[mode[i]] = args->min_partition_size_arg[0];
                result->max_partition_size_log[mode[i]] = args->max_partition_size_arg[0];

                result->min_thread_count_log[mode[i]] = args->min_thread_count_arg[0];
                result->max_thread_count_log[mode[i]] = args->max_thread_count_arg[0];
            }
        }
    } else {
        // set min/max partition sizes/thread counts to the respective values
        for (int i = 0; i < num_modes; i++) {
            result->min_partition_size_log[mode[i]] = args->min_partition_size_arg[i];
            result->max_partition_size_log[mode[i]] = args->max_partition_size_arg[i];

            result->min_thread_count_log[mode[i]] = args->min_thread_count_arg[i];
            result->max_thread_count_log[mode[i]] = args->max_thread_count_arg[i];
        }
    }

    // set send patterns
    for (int i = 0; i < num_send_patterns; i++) {
        result->send_patterns[i] = args->send_patterns_arg[i];
    }

    if (0 == num_send_patterns)
        result->send_patterns[0] = 0; // TODO use default from ggo file


    // for (int i = 0; i < sizeof(setups)/ sizeof(setups[0]); i++)
    // {
    //     if (0 == strcasecmp(name, setup_names[i])) {
    //         return &setups[i];
    //     }
    // }
    return result;
}

int config_num_test_cases(setup config, Mode mode) {
#ifdef DISABLE_PSEND
        if (is_psend(mode))
            return 0;
#endif

    printf("%i,%i,%i,%i,%i,%i\n", config->enable_mode[mode], 
    config->max_partition_size_log[mode], config->min_partition_size_log[mode], 
    config->max_thread_count_log[mode], config->min_thread_count_log[mode],
    config->num_send_patterns);
    return ((int)config->enable_mode[mode])
     * (config->num_send_patterns)
      * (is_psend(mode) ? (config->max_partition_size_log[mode] - config->min_partition_size_log[mode] + 1) : 1)
       * (config->max_partition_size_log[mode] - config->min_partition_size_log[mode] + 1)
        * (config->max_thread_count_log[mode] - config->min_thread_count_log[mode] + 1);
}

int config_min_partition_size_log_total(setup config) {
    int result = config->min_partition_size_log[0];
    for (int i = 0; i < ModeCount; ++i) {
        if (config->min_partition_size_log[i] < result) {
            result = config->min_partition_size_log[i];
        }
    }
    return result;
}

int config_max_partition_size_log_total(setup config) {
    int result = config->max_partition_size_log[0];
    for (int i = 0; i < ModeCount; ++i) {
        if (config->max_partition_size_log[i] > result) {
            result = config->max_partition_size_log[i];
        }
    }
    return result;
}

int config_min_partition_size_total(setup config) {
    return (1 << config_min_partition_size_log_total(config));
}

int config_max_partition_size_total(setup config) {
    return (1 << config_max_partition_size_log_total(config));
}

int config_max_partition_size(setup config, Mode mode) {
    return (1 << config->max_partition_size_log[mode]);
} 

int config_min_partition_size(setup config, Mode mode) {
    return (1 << config->min_partition_size_log[mode]);
}

int config_max_thread_count(setup config, Mode mode){
    return (1 << config->max_thread_count_log[mode]);
}
int config_min_thread_count(setup config, Mode mode){
    return (1 << config->min_thread_count_log[mode]);
}