#include "send_patterns.h"
#include "bench.h"
#include "test_cases.h"
#include "setups.h"

#include "cmdline.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

#include <mpi.h>
#include <time.h>
#include <stdlib.h>


// perform some virtual work on a partition
void work(const MPI_Count partition_size)
{
    // struct timespec t1, t2;
    // t1.tv_sec = 0;
    // t1.tv_nsec = partition_size / 512;
    // if (t1.tv_nsec > 0)
    //     nanosleep(&t1, &t2);
    // int t = 13;
    // for (size_t i = 0; i < partition_size; i++)
    // {
    //     t = t * t;
    // }

    return;
}

char* result_file_paths[2] = {
    "./R0.csv",
    "./R1.csv"
};
 
const char* result_file_name(int comm_rank) {
    if (comm_rank < sizeof(result_file_paths) / sizeof(char*)) {
        return result_file_paths[comm_rank];
    } else {
        return NULL;
    }
}

int result_file_exists(int comm_rank) {
    const char* fname = result_file_name(comm_rank);

    if (NULL != fname) {        // check for file existence
        return (-1 != access(fname, F_OK));
    } else {
        return 0;
    }
}

void result_file_create(int comm_rank) {
    FILE *file;

    // open file and overwrite with header line
    if (comm_rank == 0 || comm_rank == 1) {
        file = fopen(result_file_name(comm_rank), "w");

        fprintf(file, "mode,buffer_size,thread_count,partition_size,partition_size_recv,send_pattern,t_local,t_start_to_wait,t_wait,t_total,t_wait_relative,bandwidth,std_dev(t_local),std_dev(t_start_to_wait),std_dev(t_total)\n");
        fclose(file);
    } else {
        printf("no output from this rank\n");
    }
}


FILE *result_file_open(int comm_rank)
{
    FILE* file = NULL;

    // if file does not exist yet, create it and write header
    if (!result_file_exists(comm_rank)) {
        result_file_create(comm_rank);
    }

    // open in append mode
    if (comm_rank == 0 || comm_rank == 1) 
        file = fopen(result_file_name(comm_rank), "a");

    return file;
}

void record_result(TestCase *test_case, Result *result, FILE *file)
{
    fprintf(file, "%s,%lli,%i,%lli,%lli,%s,%f,%f,%f,%f,%f,%f,%f,%f,%f\n", 
                    mode_names[test_case->mode], 
                    test_case->buffer_size, 
                    test_case->thread_count,
                    test_case->partition_size, 
                    test_case->partition_size_recv, 
                    send_pattern_identifiers[test_case->send_pattern_num], 
                    result->timings[Iteration], 
                    result->timings[IterationStartToWait], 
                    result->timings[Iteration] - result->timings[IterationStartToWait],
                    result->timings[Total], 
                    (result->timings[Iteration] - result->timings[IterationStartToWait]) / result->timings[Iteration],
                    result->bandwidth, 
                    result->timings_std_dev[Iteration],
                    result->timings_std_dev[IterationStartToWait],
                    result->timings_std_dev[Total]);
    fflush(file);
}

void result_file_close(FILE *file)
{
    fclose(file);
}

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

    struct gengetopt_args_info args;
    if (cmdline_parser (argc, argv, &args) != 0) {
        cmdline_parser_print_help();
        exit(1);
    }


    setup selection = config_from_args(&args);
    if (NULL == selection) 
        return -1;

    TestCases tests;
    test_cases_init(selection, &tests);

    //
    if (comm_rank == 0)
        printf("Running %i tests, with buffer size %9lli, iteration count: %i: \n", test_cases_get_count(tests), selection->buffer_size, selection->iterations);

    // set up result file for this rank
    FILE *result_file = result_file_open(comm_rank);
    bool success = (comm_rank == 1);    // don't use success value on rank 1

    // run test cases
    for (size_t i = 0; i < test_cases_get_count(tests); i++)
    {
        TestCase *test_case = test_cases_get_test_case(tests, i);
        Result *result = test_cases_get_result(tests, i);

        if (comm_rank == 1)
        {
            if (test_case->partition_size == test_case->partition_size_recv)
                printf("Running test %.4li in mode %15s, with %i threads, partition size %7lli, send pattern %s :\n\t", i, mode_names[test_case->mode], test_case->thread_count, test_case->partition_size, send_pattern_identifiers[test_case->send_pattern_num]);
            else
                printf("Running test %.4li in mode %15s, with %i threads, partition size %7lli -> %7lli, send pattern %s :\n\t", i, mode_names[test_case->mode], test_case->thread_count, test_case->partition_size, test_case->partition_size_recv, send_pattern_identifiers[test_case->send_pattern_num]);
            fflush(stdout);
        }

        // if time limit was exceeded in last test, don't bench this mode any more
        const double time_limit = 5.0;
        if (i > 0 && test_case->mode == test_cases_get_test_case(tests, i - 1)->mode 
                  && test_case->thread_count == test_cases_get_test_case(tests, i-1)->thread_count 
                  && test_cases_get_result(tests, i - 1)->timings[Total] > time_limit) {
            *result = *test_cases_get_result(tests, i-1);

            if (comm_rank == 1){
                printf("time limit exceeded in previous run, not running benchmark\n"); 
                fflush(stdout);
            }
        } else {    // otherwise, run benchmark
            *result = bench(test_case, comm_rank, comm_size);
            usleep(10000); // sleep 10 ms

            if (comm_rank == 1) {
                printf("success = %i, total time: %10gs, average time: %10gms, standard deviation = %10g\n", result->success, result->timings[Total], result->timings[Iteration] * 1000, result->timings_std_dev[Iteration] / result->timings[Iteration]);
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
        printf("done, success: %d\n", success);
    }

    return 0;
}
