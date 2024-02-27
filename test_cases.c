
#include "send_patterns.h"
#include "bench.h"

#include "test_cases.h"

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <assert.h>

struct test_cases {
    char *buffer;
    MPI_Count buffer_size;

    // 
    const SendPattern *send_patterns;

    // 
    int test_count;

    int min_partition_size;
    int max_partition_size;

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
    return (mode == Psend || mode == PsendThreaded || mode == PsendProgress || mode == PsendParrived || mode == PsendProgressThread || mode == CustomPsend);
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

permutation* test_cases_send_pattern(struct test_cases* tests, int partition_size, int byte_send_pattern_index) {
    int size_index = 0;
    for (MPI_Count p = tests->max_partition_size; p > partition_size; p = p / 2)
        size_index++;
    assert(size_index >= 0 && size_index < tests->num_partition_sizes);
    return &tests->partition_send_patterns[size_index][byte_send_pattern_index];
}

void set_send_pattern_count(struct test_cases* tests, int num_partition_sizes, int num_byte_send_patterns) {
    
    tests->num_partition_sizes = num_partition_sizes;
    tests->num_send_patterns = num_byte_send_patterns;
    tests->partition_send_patterns = calloc(tests->num_partition_sizes, sizeof(permutation));
    {
        for (int size_index = 0; size_index < num_partition_sizes; size_index++)
        {
            tests->partition_send_patterns[size_index] = calloc(num_byte_send_patterns, sizeof(permutation));
        }
    }
}

/**
 * @brief
 *
 * @param buffer_size size of the buffer in bytes
 */
void test_cases_init(MPI_Count buffer_size, int num_repetitions, bool *use_mode, const MPI_Count *min_partition_size, const MPI_Count *max_partition_size, const SendPattern *send_patterns, int byte_send_patterns_count, TestCases* tests)
{
    struct test_cases* result = malloc(sizeof(struct test_cases));
    result->buffer = malloc(sizeof(char) * buffer_size);
    result->buffer_size = buffer_size;
    result->num_send_patterns = byte_send_patterns_count;
    result->send_patterns = send_patterns;

    // here minimum and maximum over all partition sizes are stored 
    result->min_partition_size = buffer_size;
    result->max_partition_size = 0;

    // count number of test cases
    result->test_count = 0;
    for (Mode mode = 0; mode < ModeCount; mode++) {

#ifdef DISABLE_PSEND
        use_mode[mode] &= !is_psend(mode);
#endif

        if (use_mode[mode])
        {
            if (result->max_partition_size < max_partition_size[mode])
                result->max_partition_size = max_partition_size[mode];

            if (result->min_partition_size > min_partition_size[mode])
                result->min_partition_size = min_partition_size[mode];

            if (is_psend(mode))
            {
                for (MPI_Count partition_size = max_partition_size[mode]; partition_size >= min_partition_size[mode]; partition_size = partition_size / 2)
                {
                    for (MPI_Count partition_size_recv = max_partition_size[mode]; partition_size_recv >= min_partition_size[mode]; partition_size_recv = partition_size_recv / 2)
                        result->test_count += byte_send_patterns_count;
                }
            }
            else
            {
                for (MPI_Count partition_size = max_partition_size[mode]; partition_size >= min_partition_size[mode]; partition_size = partition_size / 2) {
                    result->test_count += byte_send_patterns_count;
                }
            }
        }
    }

    // set up byte send patterns
    permutation *byte_send_patterns = calloc(byte_send_patterns_count, sizeof(permutation));
    for (size_t i = 0; i < byte_send_patterns_count; i++)
    {
        byte_send_patterns[i] = malloc(sizeof(unsigned int) * buffer_size);
        make_send_pattern(byte_send_patterns[i], buffer_size, send_patterns[i]);
        printf("Making byte send pattern %s: %d %d %d %d %d %d...\n", send_pattern_identifiers[send_patterns[i]], byte_send_patterns[i][0], byte_send_patterns[i][1], byte_send_patterns[i][2], byte_send_patterns[i][3], byte_send_patterns[i][4], byte_send_patterns[i][5]);
    }

    // set up partition send patterns
    result->num_partition_sizes = 0;
    for (MPI_Count partition_size = result->max_partition_size; partition_size >= result->min_partition_size; partition_size = partition_size / 2)
        result->num_partition_sizes++;

    {
        set_send_pattern_count(result, result->num_partition_sizes, result->num_send_patterns);
        size_t size_index = 0;
        for (MPI_Count partition_size = result->max_partition_size; partition_size >= result->min_partition_size; partition_size = partition_size / 2)
        {
            for (int i = 0; i < byte_send_patterns_count; i++)
            {
                partition_send_pattern_create(&result->partition_send_patterns[size_index][i], byte_send_patterns[i], buffer_size, partition_size);
            }
            size_index++;
        }
    }

    result->test_cases = calloc(result->test_count, sizeof(TestCase));
    result->results = calloc(result->test_count, sizeof(Result));

    size_t index = 0;
    for (Mode mode = 0; mode < ModeCount; mode++)
    {
        if (use_mode[mode])
        {

            size_t first_size_index = 0;
            for (MPI_Count partition_size = result->max_partition_size; partition_size > max_partition_size[mode]; partition_size = partition_size / 2)
                first_size_index++;
            assert(first_size_index >= 0 && first_size_index < result->num_partition_sizes);

            if (is_psend(mode))
            {
                for (MPI_Count partition_size = max_partition_size[mode]; partition_size >= min_partition_size[mode]; partition_size = partition_size / 2)
                {
                    for (MPI_Count partition_size_recv = max_partition_size[mode]; partition_size_recv >= min_partition_size[mode]; partition_size_recv = partition_size_recv / 2)
                    {
                        for (int i = 0; i < byte_send_patterns_count; i++)
                        {
                            TestCase *test_case = &result->test_cases[index++];
                            test_case->send_pattern_num = send_patterns[i];
                            test_case->send_pattern = *test_cases_send_pattern(result, partition_size, i);
                            test_case->recv_pattern = *test_cases_send_pattern(result, partition_size_recv, i);
                            test_case->mode = mode;
                            test_case->method = mode_methods[test_case->mode];
                            test_case->iteration_count = num_repetitions;
                            test_case->buffer_size = result->buffer_size;
                            test_case->buffer = result->buffer;
                            test_case->partition_size = partition_size;
                            test_case->partition_size_recv = partition_size_recv;
                            test_case->partition_count = result->buffer_size / partition_size;
                            test_case->partition_count_recv = result->buffer_size / partition_size_recv;
                        }
                    }
                }
            } else {
                for (MPI_Count partition_size = max_partition_size[mode]; partition_size >= min_partition_size[mode]; partition_size = partition_size / 2)
                {
                    for (int i = 0; i < byte_send_patterns_count; i++)
                    {
                        TestCase *test_case = &result->test_cases[index++];
                        test_case->send_pattern_num = send_patterns[i];
                        test_case->send_pattern = *test_cases_send_pattern(result, partition_size, i);
                        test_case->recv_pattern = *test_cases_send_pattern(result, partition_size, i);
                        test_case->mode = mode;
                        test_case->method = mode_methods[test_case->mode];
                        test_case->iteration_count = num_repetitions;
                        test_case->buffer_size = result->buffer_size;
                        test_case->buffer = result->buffer;
                        test_case->partition_size = partition_size;
                        test_case->partition_size_recv = partition_size;
                        test_case->partition_count = result->buffer_size / partition_size;
                        test_case->partition_count_recv = result->buffer_size / partition_size;
                    }
                }
            }
        }
    }

    result->test_count = index;

    // byte send patterns are not needed anymore
    for (size_t i = 0; i < byte_send_patterns_count; i++)
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


void timers_store(timer *timers, Result *result)
{
    for (int i = 0; i < TimerCount; i++) {
        result->timings[i] = timers[i].sum;
        result->timings_std_dev[i] = timer_std_dev(&timers[i]);
    }
}