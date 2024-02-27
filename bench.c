#include "send_patterns.h"
#include "bench.h"
#include "test_cases.h"
#include "setups.h"

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



FILE *open_result_file(int comm_rank)
{
    FILE *file;

    // open file and overwrite with header line
    if (comm_rank == 0)
        file = fopen("./R0.csv", "w");
    else if (comm_rank == 1)
        file = fopen("./R1.csv", "w");
    else
    {
        printf("no output from this rank\n");
        return NULL;
    }
    fprintf(file, "mode,buffer_size,partition_size,partition_size_recv,send_pattern,t_local,t_start_to_wait,t_wait,t_total,t_wait_relative,bandwidth,std_dev(t_local),std_dev(t_start_to_wait),std_dev(t_total)\n");
    fclose(file);

    // open in append mode
    if (comm_rank == 0)
        file = fopen("./R0.csv", "a");
    else if (comm_rank == 1)
        file = fopen("./R1.csv", "a");

    return file;
}

void record_result(TestCase *test_case, Result *result, FILE *file)
{
    fprintf(file, "%s,%lli,%lli,%lli,%s,%f,%f,%f,%f,%f,%f,%f,%f,%f\n", 
                    mode_names[test_case->mode], 
                    test_case->buffer_size, 
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

void close_result_file(FILE *file)
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

    if (comm_size != 2)
        return -1;

    setup selection = select_setup(argv[1]);
    if (NULL == selection) 
        return -1;

    TestCases tests;
    test_cases_init(selection->buffer_size, selection->iterations, selection->enable_mode, selection->min_partition_size, selection->max_partition_size, selection->send_patterns, selection->num_send_patterns, &tests);

    //
    if (comm_rank == 0)
        printf("Running %i tests, with buffer size %9lli, iteration count: %i: \n", test_cases_get_count(tests), selection->buffer_size, selection->iterations);

    // set up result file for this rank
    FILE *result_file = open_result_file(comm_rank);
    bool success = (comm_rank == 1);  // don't use success value on rank 1

    // run test cases
    for (size_t i = 0; i < test_cases_get_count(tests); i++)
    {
        TestCase *test_case = test_cases_get_test_case(tests, i);
        Result *result = test_cases_get_result(tests, i);

        if (comm_rank == 0)
        {
            if (test_case->partition_size == test_case->partition_size_recv)
                printf("Running test %.4li in mode %15s, partition size %7lli, send pattern %s :\n\t", i, mode_names[test_case->mode], test_case->partition_size, send_pattern_identifiers[test_case->send_pattern_num]);
            else
                printf("Running test %.4li in mode %15s, partition size %7lli -> %7lli, send pattern %s :\n\t", i, mode_names[test_case->mode], test_case->partition_size, test_case->partition_size_recv, send_pattern_identifiers[test_case->send_pattern_num]);
            fflush(stdout);
        }

        // if time limit was exceeded in last test, don't bench this mode any more
        const double time_limit = 5.0;
        if (i > 0 && test_case->mode == test_cases_get_test_case(tests, i - 1)->mode && test_cases_get_result(tests, i - 1)->timings[Total] > time_limit) {
            *result = *test_cases_get_result(tests, i-1);

            if (comm_rank == 0){
                printf("time limit exceeded in previous run, not running benchmark\n"); 
                fflush(stdout);
            }
        } else {    // otherwise, run benchmark
            *result = bench(test_case, comm_rank, comm_size);

            if (comm_rank == 1) {
                printf("success = %i, total time: %10gs, average time: %10gμs, standard deviation = %10g\n", result->success, result->timings[Total], result->timings[Iteration], result->timings_std_dev[Iteration]);
                fflush(stdout);
            }
        }

        record_result(test_case, result, result_file);

        success &= result->success;
    }

    close_result_file(result_file);
    test_cases_free(&tests);

    MPI_Finalize();

    if (comm_rank == 1)
    {
        printf("done, success: %d\n", success);
    }

    return 0;
}
