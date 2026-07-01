#include "setups.h"

#include <math.h>
#include "cmdline.h"

setup make_setup(struct gengetopt_args_info* args)
{
    Mode mode = 0;
    for (int i = 0; i < ModeCount; i++)
    {
        if (0 == strcmp(mode_names[i], args->mode_arg))
            mode = i;
    }

    SendPattern send_pattern = 0;
    for (int i = 0; i < SendPatternCount; i++)
    {
        if (0 == strcmp(send_pattern_identifiers[i], args->send_pattern_arg))
            send_pattern = i;
    }

    struct setup_t result = {
        .num_test_cases = 0,
        .buffer_size = args->num_values_arg,
        .iterations = args->iterations_arg,
        .mode = mode,
        .min_partition_size = args->partition_size_arg,
        .max_partition_size = args->partition_size_arg,
        .min_thread_count = args->num_threads_arg,
        .max_thread_count = args->num_threads_arg,
        .send_patterns = { send_pattern },
        .num_send_patterns = 1
    };

    if (is_psend(mode)) {
        for (int i = result.max_partition_size; i >= result.min_partition_size; i /= 2)
        for (int j = result.max_partition_size; j >= result.min_partition_size; j /= 2)
        for (int t = result.min_thread_count; t <= result.max_thread_count; t *= 2)
            result.num_test_cases++;
    } else {
        for (int j = result.max_partition_size; j >= result.min_partition_size; j /= 2)
        for (int t = result.min_thread_count; t <= result.max_thread_count; t *= 2)
            result.num_test_cases++;
    }

    setup ptr = calloc(sizeof(struct setup_t), 1);
    *ptr = result;

    return ptr;
}

int num_test_cases(setup config, Mode mode) {
#ifdef DISABLE_PSEND
        if (is_psend(mode))
            return 0;
#endif

    return (mode == config->mode) * config->num_test_cases;
}
