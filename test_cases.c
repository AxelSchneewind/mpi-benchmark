
#include "send_patterns.h"
#include "bench.h"

#include "test_cases.h"
#include "setups.h"

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>

struct test_cases {
    char *buffer;
    MPI_Count buffer_size;

    // 
    const SendPattern *send_patterns;

    // 
    int test_count;

    int min_partition_size;
    int max_partition_size;

    int min_partition_size_log;
    int max_partition_size_log;

    // 
    TestCase *test_cases;
    Result *results;

    // TODO make more readable
    // [index of partition size][index of pattern]
    permutation** partition_send_patterns;
    int num_partition_sizes;
    int num_send_patterns;
};
typedef struct test_cases* TestCases;


int is_psend(Mode mode)
{
    return (mode == Psend || mode == PsendProgress || mode == PsendParrived || mode == PsendProgressThread || mode == CustomPsend);
}

void make_send_pattern(permutation result, size_t count, SendPattern pattern)
{
    switch (pattern)
    {
    case Linear:
        make_linear_pattern(result, count);
        break;
    case LinearInverse:
        make_linear_inverse_pattern(result, count);
        break;
    case Stride1K:
        make_stride_pattern(result, count, 1024);
        break;
    case Stride16K:
        make_stride_pattern(result, count, 16*1024);
        break;
    case Stride128:
        make_stride_pattern(result, count, 128);
        break;
    case Random:
        make_random_pattern(result, count);
        break;
    case RandomBurst128:
        make_random_burst_pattern(result, count, 128);
        break;
    case RandomBurst1K:
        make_random_burst_pattern(result, count, 1024);
        break;
    case RandomBurst16K:
        make_random_burst_pattern(result, count, 16*1024);
        break;
    
    default:
        break;
    }
}



int test_cases_get_count(TestCases tests)
{
    return tests->test_count;
}

TestCase *test_cases_get_test_case(TestCases tests, int i)
{
    return &tests->test_cases[i];
}
Result *test_cases_get_result(TestCases tests, int i)
{
    return &tests->results[i];
}

void partition_send_pattern_create(permutation* result_ptr, const permutation byte_send_pattern, int buffer_size, int partition_size) {
    permutation_create(result_ptr, buffer_size / partition_size);
    make_partition_send_pattern(byte_send_pattern, *result_ptr, buffer_size, partition_size);
}

permutation test_cases_send_pattern(struct test_cases* tests, int partition_size, int byte_send_pattern_index) {
    int size_index = 0;
    for (MPI_Count p = tests->max_partition_size; p > partition_size; p = p / 2)
        size_index++;
    assert(size_index >= 0 && size_index < tests->num_partition_sizes);
    return tests->partition_send_patterns[size_index][byte_send_pattern_index];
}

void set_send_pattern_count(struct test_cases* tests, int num_partition_sizes, int num_byte_send_patterns) {
    
    tests->num_partition_sizes = num_partition_sizes;
    tests->num_send_patterns = num_byte_send_patterns;
    tests->partition_send_patterns = calloc(tests->num_partition_sizes, sizeof(permutation));
    for (int size_index = 0; size_index < num_partition_sizes; size_index++)
    {
        tests->partition_send_patterns[size_index] = calloc(num_byte_send_patterns, sizeof(permutation));
    }
}

/**
 * @brief
 *
 * @param buffer_size size of the buffer in bytes
 */
void test_cases_init(setup configuration, TestCases* tests)
{
    struct test_cases* result = malloc(sizeof(struct test_cases));
    result->buffer = malloc(sizeof(char) * configuration->buffer_size);
    result->buffer_size = configuration->buffer_size;
    result->num_send_patterns = configuration->num_send_patterns;
    result->send_patterns = configuration->send_patterns;

    // here minimum and maximum over all partition sizes are stored 
    result->min_partition_size = setup_min_partition_size_total(configuration);
    result->max_partition_size = setup_max_partition_size_total(configuration);

    result->min_partition_size_log = setup_min_partition_size_log_total(configuration);
    result->max_partition_size_log = setup_max_partition_size_log_total(configuration);

    // count number of test cases
    result->test_count = 0;
    for (Mode mode = 0; mode < ModeCount; mode++) {
        result->test_count += num_test_cases(configuration, mode);
    }

    // set up byte send patterns
    permutation *byte_send_patterns = calloc(configuration->num_send_patterns, sizeof(permutation));
    for (size_t i = 0; i < configuration->num_send_patterns; i++)
    {
        byte_send_patterns[i] = calloc(sizeof(unsigned int), configuration->buffer_size);
        make_send_pattern(byte_send_patterns[i], configuration->buffer_size, configuration->send_patterns[i]);
        printf("Making byte send pattern %s: %d %d %d %d %d %d...\n", send_pattern_identifiers[configuration->send_patterns[i]], byte_send_patterns[i][0], byte_send_patterns[i][1], byte_send_patterns[i][2], byte_send_patterns[i][3], byte_send_patterns[i][4], byte_send_patterns[i][5]);
    }

    // set up partition send patterns
    {
        result->num_partition_sizes = result->max_partition_size_log - result->min_partition_size_log + 1;
        set_send_pattern_count(result, result->num_partition_sizes, result->num_send_patterns);
        size_t size_index = 0;
        for (MPI_Count partition_size = result->max_partition_size; partition_size >= result->min_partition_size; partition_size /= 2)
        {
            for (int i = 0; i < result->num_send_patterns; i++)
            {
                partition_send_pattern_create(&result->partition_send_patterns[size_index][i], byte_send_patterns[i], result->buffer_size, partition_size);
            }
            size_index++;
        }
    }

    result->test_cases = calloc(result->test_count, sizeof(TestCase));
    result->results = calloc(result->test_count, sizeof(Result));

    size_t index = 0;
    int max_num_threads = 1;
    // iterate over modes
    for (Mode mode = 0; mode < ModeCount; mode++) {
        // only use enabled modes
        if (configuration->enable_mode[mode]) {
            // iterate over send side partition sizes
            for (MPI_Count partition_size = setup_max_partition_size(configuration, mode); partition_size >= setup_min_partition_size(configuration, mode); partition_size /= 2) {
                // iterate over receive side partition sizes
                for (MPI_Count partition_size_recv = (is_psend(mode) ? setup_max_partition_size(configuration, mode) : partition_size); partition_size_recv >= (is_psend(mode) ? setup_min_partition_size(configuration, mode) : partition_size); partition_size_recv /= 2) {
                    // iterate over thread count
                    for (int t = setup_min_thread_count(configuration, mode); t <= setup_max_thread_count(configuration, mode); t *= 2) {
                        // iterate over send patterns
                        for (int i = 0; i < configuration->num_send_patterns; i++) {
                            if (t > result->buffer_size / partition_size)
                                continue;

                            TestCase *test_case = &result->test_cases[index++];

                            // assign send pattern
                            test_case->send_pattern_num = configuration->send_patterns[i];
                            test_case->send_pattern = test_cases_send_pattern(result, partition_size, i);
                            test_case->recv_pattern = test_cases_send_pattern(result, partition_size_recv, i);
                            assert(NULL != test_case->send_pattern && NULL != test_case->recv_pattern);

                            test_case->mode = mode;
                            test_case->method = mode_methods[test_case->mode];

                            test_case->iteration_count = configuration->iterations;

                            test_case->buffer_size = result->buffer_size;
                            test_case->buffer = result->buffer;

                            test_case->partition_size = partition_size;
                            test_case->partition_size_recv = partition_size_recv;
                            test_case->partition_count = result->buffer_size / test_case->partition_size;
                            test_case->partition_count_recv = result->buffer_size / test_case->partition_size_recv;

                            // distribute partitions over threads
                            test_case->thread_count = t;
                            if (test_case->partition_count < test_case->thread_count) {
                                printf("something went wrong\n");
                                test_case->thread_count = test_case->partition_count;
                            }
                            if (test_case->thread_count > max_num_threads)
                                max_num_threads = test_case->thread_count;
                            test_case->partitions_per_thread = test_case->partition_count / test_case->thread_count;
                            assert(test_case->partitions_per_thread * test_case->thread_count == test_case->partition_count);
                        }
                    }
                }
            }
        }
    }

    result->test_count = index;

    // create threads
    #pragma omp parallel for
    for (int t = 0; t < max_num_threads; t++) { }

    // byte send patterns are not needed anymore
    for (size_t i = 0; i < configuration->num_send_patterns; i++)
        free(byte_send_patterns[i]);
    free(byte_send_patterns);

    *tests = result;
}

void _test_cases_free(struct test_cases* tests)
{
    // 
    for (int p = 0; p < tests->num_partition_sizes; p++) {
        for (int s = 0; s < tests->num_send_patterns; s++) {
            if (NULL != tests->partition_send_patterns[p][s])
                free(tests->partition_send_patterns[p][s]);
        }
        free(tests->partition_send_patterns[p]);
    }
    free(tests->partition_send_patterns);
    tests->partition_send_patterns = NULL;

    free(tests->buffer);
    free(tests->test_cases);
    free(tests->results);
}

void test_cases_free(TestCases* tests)
{
    struct test_cases* test_ptr = *tests;
    _test_cases_free(test_ptr);
    *tests = NULL;
}


void timers_store(timers timers, Result *result)
{
    for (int i = 0; i < TimerCount; i++) {
        result->timings[i] = timer_mean(timers_get(timers, i));
        result->timings_std_dev[i] = timer_std_dev(timers_get(timers,i));
    }
}