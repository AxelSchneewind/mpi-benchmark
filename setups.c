#include "setups.h"

#include "bench.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>


setup config_from_args(struct gengetopt_args_info* args) {
    setup result = malloc(sizeof(struct setup_t));
    int mode[ModeCount];

    memset(result, 0, sizeof(struct setup_t));
    memset(mode, 0, sizeof(int) * ModeCount);
    memset(result->different_partition_sizes, args->different_partition_sizes_flag, sizeof(bool) * ModeCount);

    const int num_modes = args->modes_given;
    const int num_send_patterns = args->send_patterns_given;

    result->name = (args->bench_name_arg && strlen(args->bench_name_arg)) ? args->bench_name_arg : "bench";
    result->iterations = args->iteration_count_arg;
    result->buffer_size = (1 << args->buffer_size_arg);
    result->num_send_patterns = (0 >= num_send_patterns) ? 1 : num_send_patterns;

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
        result->send_patterns[i] = (SendPattern)args->send_patterns_arg[i];
    }

    if (0 == num_send_patterns) {
        result->send_patterns[0] = 0; // TODO use default from ggo file
    }

    return result;
}


const char* config_name(setup config) {
    return config->name;
}


int config_num_test_cases(setup config, Mode mode) {
#ifdef DISABLE_PSEND
        if (is_psend(mode))
            return 0;
#endif

    // printf("mode %i enabled: %i, lp_max: %i, lp_min: %i, lt_max: %i, lt_min: %i, num send patterns: %i\n", mode, config->enable_mode[mode], 
    // config->max_partition_size_log[mode], config->min_partition_size_log[mode], 
    // config->max_thread_count_log[mode], config->min_thread_count_log[mode],
    // config->num_send_patterns);
    return ((int)config->enable_mode[mode])
     * (config->num_send_patterns)
      * (is_psend(mode) ? (config->max_partition_size_log[mode] - config->min_partition_size_log[mode] + 1) : 1)
       * (config->max_partition_size_log[mode] - config->min_partition_size_log[mode] + 1)
        * (config->max_thread_count_log[mode] - config->min_thread_count_log[mode] + 1);
}

int config_allow_different_partition_sizes(setup config, Mode mode) {
    return config->different_partition_sizes[mode];
}


int config_min_partition_size_log_total(setup config) {
    int result = config->min_partition_size_log[0];
    for (int i = 0; i < ModeCount; ++i) {
        if (config->enable_mode[i] && config->min_partition_size_log[i] < result) {
            result = config->min_partition_size_log[i];
        }
    }
    return result;
}

int config_max_partition_size_log_total(setup config) {
    int result = config->max_partition_size_log[0];
    for (int i = 0; i < ModeCount; ++i) {
        if (config->enable_mode[i] && config->max_partition_size_log[i] > result) {
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