
#include "send_patterns.h"
#include "bench.h"
#include "test_cases.h"
#include "configuration.h"
#include "output.h"

#include "cmdline.h"

#include <mpi.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include <assert.h>

// naive check that send pattern enum from gengetopt matches enum SendPattern
static_assert((int)GridBoundary == (int)send_patterns_arg_GridBoundary, "mismatch between enum SendPattern and send_patterns_arg");

void init_buffer(char* buffer, int size) {
    srand(234);
    for (size_t i = 0; i < size; i++)
    {
        buffer[i] = (char)(rand());
    }
}

bool check_buffer(char* buffer, int size) {
    srand(234);
    for (size_t i = 0; i < size; i++) {
        if (buffer[i] != ((char)rand())) {
            return false;
        }
    }
    return true;
}


Result bench(TestCase *test_case, int comm_rank, int comm_size)
{
    static char* buffer_contents = NULL;
    if (NULL == buffer_contents)  {
        buffer_contents = malloc(test_case->buffer_size);
        init_buffer(buffer_contents, test_case->buffer_size);
    }

    // init send buffer randomly and recv buffer with 0s
    if (comm_rank == 0) {
        memcpy(test_case->buffer, buffer_contents, test_case->buffer_size);
    } else {
        memset(test_case->buffer, 0, test_case->buffer_size * sizeof(char));
    }

    // call run method for this test case
    Result result;
    if (test_case->method.run != NULL)
        (*test_case->method.run)(test_case, &result, comm_rank);

    // compute bandwidth
    result.bandwidth = ((double)test_case->buffer_size * test_case->iteration_count) / result.timings[Total];

    // check that data was transmitted correctly
    result.success = !memcmp(test_case->buffer, buffer_contents, test_case->buffer_size);

    return result;
}

int main(int argc, char **argv)
{
    struct gengetopt_args_info args;
    if (cmdline_parser (argc, argv, &args) != 0) {
        cmdline_parser_print_help();
        exit(1);
    }

    int thread_support;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &thread_support);
    if (thread_support != MPI_THREAD_MULTIPLE)
        return -1;

    int comm_rank, comm_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &comm_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);

    if (comm_size != 2) {
        exit(1);
    }

    configuration config = config_from_args(&args);
    if (NULL == config) 
        return -1;

    TestCases tests;
    test_cases_init(config, &tests);

    //
    if (comm_rank == 0)
        printf("[%5s] Running %i tests, with buffer size %9lli, iteration count: %i: \n", config_name(config), test_cases_get_count(tests), config_buffer_size(config), config_iterations(config));

    // set up result file for this rank
    FILE *result_file = result_file_open(args.output_file_arg, args.output_file_given, comm_rank);
    bool success = (comm_rank == 1);    // don't use success value on rank 1

    // run test cases
    for (size_t i = 0; i < test_cases_get_count(tests); i++)
    {
        TestCase *test_case = test_cases_get_test_case(tests, i);
        Result *result = test_cases_get_result(tests, i);

        if (comm_rank == 1)
        {
            if (test_case->partition_size == test_case->partition_size_recv)
                printf("[%5s:%.4li]%15s, %i threads, part. size %7lli, pattern %s :\n", config_name(config), i, mode_names[test_case->mode], test_case->thread_count, test_case->partition_size, send_pattern_identifiers[test_case->send_pattern_num]);
            else
                printf("[%5s:%.4li]%15s, %i threads, part. size %7lli -> %7lli, pattern %s :\n", config_name(config), i, mode_names[test_case->mode], test_case->thread_count, test_case->partition_size, test_case->partition_size_recv, send_pattern_identifiers[test_case->send_pattern_num]);
            fflush(stdout);
        }

        // if time limit was exceeded in last test, don't bench this mode any more
        const double time_limit = 5.0;
        if (i > 0 && test_case->mode == test_cases_get_test_case(tests, i - 1)->mode 
                  && test_case->thread_count == test_cases_get_test_case(tests, i-1)->thread_count 
                  && test_cases_get_result(tests, i - 1)->timings[Total] > time_limit) {
            *result = *test_cases_get_result(tests, i-1);

            if (comm_rank == 1){
                printf("            time limit exceeded in previous run, skipping benchmark\n"); 
                fflush(stdout);
            }
        } else {    // otherwise, run benchmark
            *result = bench(test_case, comm_rank, comm_size);
            usleep(10000); // sleep 10 ms

            if (comm_rank == 1) {
                printf("            %s, bandwidth: %6.1lfGiB/s, avg time: %6.1lfms, std-deviation = %6.1lfms\n", result->success ? "success" : "failure", result->bandwidth/1024/1024/1024, result->timings[Iteration] * 1000, result->timings_std_dev[Iteration] * 1000);
                fflush(stdout);
            }
        }

        record_result(test_case, result, result_file);

        success &= result->success;
    }

    result_file_close(result_file);
    test_cases_free(&tests);

    MPI_Finalize();

    if (comm_rank == 1)
    {
        printf("done, %s\n", success ? "all tests sucessful" : "some tests failed");
    }

    return 0;
}
