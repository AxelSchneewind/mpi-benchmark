#include "bench.h"
#include "stdlib.h"
#include "stdbool.h"
#include "send_patterns.h"

// TODO move into struct and store pointers to partition send patterns there
static char *buffer;
static int *send_pattern;
static int test_count;
static TestCase *test_cases;
static Result *results;

int test_cases_get_count()
{
    return test_count;
}

/**
 * @brief
 *
 * @param buffer_size size of the buffer in bytes
 */
void test_cases_init(MPI_Count buffer_size, int num_repetitions, const bool *use_mode, const MPI_Count *min_partition_size, const MPI_Count *max_partition_size, const SendPattern *send_patterns, int byte_send_patterns_count)
{
    buffer = malloc(buffer_size);

    size_t global_min_partition_size = buffer_size;
    size_t global_max_partition_size = 0;

    // count number of test cases
    test_count = 0;
    for (Mode mode = 0; mode < ModeCount; mode++)
        if (use_mode[mode])
        {
            if (global_max_partition_size < max_partition_size[mode])
                global_max_partition_size = max_partition_size[mode];

            if (global_min_partition_size > min_partition_size[mode])
                global_min_partition_size = min_partition_size[mode];

            if (mode == Psend || mode == PsendThreaded || mode == PsendProgress)
            {
                for (MPI_Count partition_size = max_partition_size[mode]; partition_size >= min_partition_size[mode]; partition_size = partition_size / 2)
                {
                    for (MPI_Count partition_size_recv = max_partition_size[mode]; partition_size_recv >= min_partition_size[mode]; partition_size_recv = partition_size_recv / 2)
                        test_count += byte_send_patterns_count;
                }
            }
            else
            {
                for (MPI_Count partition_size = max_partition_size[mode]; partition_size >= min_partition_size[mode]; partition_size = partition_size / 2)
                    test_count += byte_send_patterns_count;
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
    size_t num_partition_sizes = 0;
    for (MPI_Count partition_size = global_max_partition_size; partition_size >= global_min_partition_size; partition_size = partition_size / 2)
        num_partition_sizes++;

    unsigned int ***partition_send_patterns = malloc(sizeof(unsigned int *) * num_partition_sizes);
    {
        size_t size_index = 0;
        for (MPI_Count partition_size = global_max_partition_size; partition_size >= global_min_partition_size; partition_size = partition_size / 2)
        {
            partition_send_patterns[size_index] = malloc(sizeof(unsigned int *) * byte_send_patterns_count);
            for (int i = 0; i < byte_send_patterns_count; i++)
            {
                partition_send_patterns[size_index][i] = malloc(sizeof(unsigned int) * buffer_size / partition_size);
                printf("from byte send pattern %20s: %i %i %i %i %i %i...\n", send_pattern_identifiers[i], byte_send_patterns[i][0], byte_send_patterns[i][1], byte_send_patterns[i][2], byte_send_patterns[i][3], byte_send_patterns[i][4], byte_send_patterns[i][5]);
                make_partition_send_pattern(byte_send_patterns[i], partition_send_patterns[size_index][i], buffer_size, partition_size);
                printf(" making partition send pattern for size %i: %i %i %i %i %i %i %i %i...\n", partition_size, partition_send_patterns[size_index][i][0],partition_send_patterns[size_index][i][1],partition_send_patterns[size_index][i][2],partition_send_patterns[size_index][i][3],partition_send_patterns[size_index][i][4],partition_send_patterns[size_index][i][5],partition_send_patterns[size_index][i][6],partition_send_patterns[size_index][i][7],partition_send_patterns[size_index][i][8]);
            }
            size_index++;
        }
    }

    test_cases = malloc(sizeof(TestCase) * test_count);
    results = malloc(sizeof(Result) * test_count);

    size_t index = 0;
    for (Mode mode = 0; mode < ModeCount; mode++)
    {
        if (use_mode[mode])
        {

            size_t first_size_index = 0;
            for (MPI_Count partition_size = global_max_partition_size; partition_size > max_partition_size[mode]; partition_size = partition_size / 2)
                first_size_index += 4;

            if (mode == Psend || mode == PsendThreaded || mode == PsendProgress)
            {
                size_t pattern_index = first_size_index;
                for (MPI_Count partition_size = max_partition_size[mode]; partition_size >= min_partition_size[mode]; partition_size = partition_size / 2)
                {
                    for (MPI_Count partition_size_recv = max_partition_size[mode]; partition_size_recv >= min_partition_size[mode]; partition_size_recv = partition_size_recv / 2)
                    {
                        for (int i = 0; i < byte_send_patterns_count; i++)
                        {
                            TestCase *test_case = &test_cases[index++];
                            test_case->send_pattern_num = i;
                            test_case->send_pattern = partition_send_patterns[pattern_index][i];
                            test_case->mode = mode;
                            test_case->method = mode_methods[test_case->mode];
                            test_case->iteration_count = num_repetitions;
                            test_case->buffer_size = buffer_size;
                            test_case->buffer = buffer;
                            test_case->partition_size = partition_size;
                            test_case->partition_size_recv = partition_size_recv;
                            test_case->partition_count = buffer_size / partition_size;
                            test_case->partition_count_recv = buffer_size / partition_size_recv;
                        }
                    }

                    pattern_index++;
                }
            }
            else
            {
                size_t pattern_index = first_size_index;
                for (MPI_Count partition_size = max_partition_size[mode]; partition_size >= min_partition_size[mode]; partition_size = partition_size / 2)
                {
                    for (int i = 0; i < byte_send_patterns_count; i++)
                    {
                        TestCase *test_case = &test_cases[index++];
                        test_case->send_pattern_num = i;
                        test_case->send_pattern = partition_send_patterns[pattern_index][i];
                        test_case->mode = mode;
                        test_case->method = mode_methods[test_case->mode];
                        test_case->iteration_count = num_repetitions;
                        test_case->buffer_size = buffer_size;
                        test_case->buffer = buffer;
                        test_case->partition_size = partition_size;
                        test_case->partition_size_recv = partition_size;
                        test_case->partition_count = buffer_size / partition_size;
                        test_case->partition_count_recv = buffer_size / partition_size;
                    }
                    pattern_index++;
                }
            }
        }
    }

    test_count = index;

    // byte send patterns are not needed anymore
    for (size_t i = 0; i < byte_send_patterns_count; i++)
        free(byte_send_patterns[i]);
    free(byte_send_patterns);
}

void test_cases_free()
{
    for (size_t i = 0; i < test_count; i++)
    {
        // free(test_cases[i].send_pattern);
    }

    free(buffer);
    free(send_pattern);
    free(test_cases);
    free(results);
}

int get_test_count() { return test_count; }

TestCase *get_test_case(int i)
{
    return &test_cases[i];
}
Result *get_result(int i)
{
    return &results[i];
}
