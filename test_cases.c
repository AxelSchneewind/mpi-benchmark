#include "bench.h"
#include "send_patterns.h"

#include "test_cases.h"

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

struct test_cases {
    char *buffer;
    MPI_Count buffer_size;

    // 
    int *send_pattern;

    // 
    int test_count;

    // 
    TestCase *test_cases;
    Result *results;

    // TODO make more readable
    // [index of partition size][index of pattern]
    unsigned int ***partition_send_patterns;
    int num_partition_sizes;
    int num_send_patterns;
};
typedef struct test_cases* TestCases;



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


/**
 * @brief
 * TODO cleanup this mess
 *
 * @param buffer_size size of the buffer in bytes
 */
void test_cases_init(MPI_Count buffer_size, int num_repetitions, const bool *use_mode, const MPI_Count *min_partition_size, const MPI_Count *max_partition_size, const SendPattern *send_patterns, int byte_send_patterns_count, TestCases* tests)
{
    struct test_cases* result = malloc(sizeof(struct test_cases));
    result->buffer = malloc(buffer_size);
    result->buffer_size = buffer_size;
    result->num_send_patterns = byte_send_patterns_count;

    // here minimum and maximum over all partition sizes are stored 
    size_t global_min_partition_size = buffer_size;
    size_t global_max_partition_size = 0;

    // count number of test cases
    result->test_count = 0;
    for (Mode mode = 0; mode < ModeCount; mode++) {

#ifdef DISABLE_PSEND
        use_mode[mode] &= !is_psend(mode);
#endif

        if (use_mode[mode])
        {
            if (global_max_partition_size < max_partition_size[mode])
                global_max_partition_size = max_partition_size[mode];

            if (global_min_partition_size > min_partition_size[mode])
                global_min_partition_size = min_partition_size[mode];

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
    unsigned int **byte_send_patterns = malloc(sizeof(unsigned int *) * byte_send_patterns_count);
    for (size_t i = 0; i < byte_send_patterns_count; i++)
    {
        byte_send_patterns[i] = malloc(sizeof(unsigned int) * buffer_size);
        make_send_pattern(byte_send_patterns[i], buffer_size, send_patterns[i]);
        printf("Making byte send pattern %s: %i %i %i %i %i %i...\n", send_pattern_identifiers[i], byte_send_patterns[i][0], byte_send_patterns[i][1], byte_send_patterns[i][2], byte_send_patterns[i][3], byte_send_patterns[i][4], byte_send_patterns[i][5]);
    }

    // set up partition send patterns
    result->num_partition_sizes = 0;
    for (MPI_Count partition_size = global_max_partition_size; partition_size >= global_min_partition_size; partition_size = partition_size / 2)
        result->num_partition_sizes++;

    result->partition_send_patterns = malloc(sizeof(unsigned int *) * result->num_partition_sizes);
    {
        size_t size_index = 0;
        for (MPI_Count partition_size = global_max_partition_size; partition_size >= global_min_partition_size; partition_size = partition_size / 2)
        {
            result->partition_send_patterns[size_index] = malloc(sizeof(unsigned int *) * byte_send_patterns_count);
            for (int i = 0; i < byte_send_patterns_count; i++)
            {
                result->partition_send_patterns[size_index][i] = malloc(sizeof(unsigned int) * buffer_size / partition_size);
                // printf("from byte send pattern %20s: %i %i %i %i %i %i...\n", send_pattern_identifiers[i], byte_send_patterns[i][0], byte_send_patterns[i][1], byte_send_patterns[i][2], byte_send_patterns[i][3], byte_send_patterns[i][4], byte_send_patterns[i][5]);
                make_partition_send_pattern(byte_send_patterns[i], result->partition_send_patterns[size_index][i], buffer_size, partition_size);
                // printf(" making partition send pattern for size %10lli: %i %i %i %i %i %i %i %i %i...\n", partition_size, partition_send_patterns[size_index][i][0],partition_send_patterns[size_index][i][1],partition_send_patterns[size_index][i][2],partition_send_patterns[size_index][i][3],partition_send_patterns[size_index][i][4],partition_send_patterns[size_index][i][5],partition_send_patterns[size_index][i][6],partition_send_patterns[size_index][i][7],partition_send_patterns[size_index][i][8]);
            }
            size_index++;
        }
    }

    result->test_cases = malloc(sizeof(TestCase) * result->test_count);
    result->results = malloc(sizeof(Result) * result->test_count);

    size_t index = 0;
    for (Mode mode = 0; mode < ModeCount; mode++)
    {
        if (use_mode[mode])
        {

            size_t first_size_index = 0;
            for (MPI_Count partition_size = global_max_partition_size; partition_size > max_partition_size[mode]; partition_size = partition_size / 2)
                first_size_index += 4;

            if (is_psend(mode))
            {
                size_t pattern_index = first_size_index;
                for (MPI_Count partition_size = max_partition_size[mode]; partition_size >= min_partition_size[mode]; partition_size = partition_size / 2)
                {
                    for (MPI_Count partition_size_recv = max_partition_size[mode]; partition_size_recv >= min_partition_size[mode]; partition_size_recv = partition_size_recv / 2)
                    {
                        for (int i = 0; i < byte_send_patterns_count; i++)
                        {
                            TestCase *test_case = &result->test_cases[index++];
                            test_case->send_pattern_num = i;
                            test_case->send_pattern = result->partition_send_patterns[pattern_index][i];
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

                    pattern_index++;
                }
            } else {
                size_t pattern_index = first_size_index;
                for (MPI_Count partition_size = max_partition_size[mode]; partition_size >= min_partition_size[mode]; partition_size = partition_size / 2)
                {
                    for (int i = 0; i < byte_send_patterns_count; i++)
                    {
                        TestCase *test_case = &result->test_cases[index++];
                        test_case->send_pattern_num = i;
                        test_case->send_pattern = result->partition_send_patterns[pattern_index][i];
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
                    pattern_index++;
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
            free(tests->partition_send_patterns[p][s]);
        }
        free(tests->partition_send_patterns[p]);
    }
    free(tests->partition_send_patterns);
    tests->partition_send_patterns = NULL;

    free(tests->buffer);
    free(tests->send_pattern);
    free(tests->test_cases);
    free(tests->results);
}

void test_cases_free(TestCases* tests)
{
    struct test_cases* test_ptr = *tests;
    _test_cases_free(test_ptr);
    *tests = NULL;
}
