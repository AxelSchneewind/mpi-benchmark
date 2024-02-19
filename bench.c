#include "send_patterns.h"
#include "bench.h"
#include "test_cases.h"
#include "timer.h"

#include "mpi.h"


#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "unistd.h"
#include "math.h"

#include <time.h>
#include <stdlib.h>

// perform some virtual work on a partition
void work(const MPI_Count partition_size)
{
    /*struct timespec t1, t2;
    t1.tv_sec = 0;
    t1.tv_nsec = partition_size / 64;
    nanosleep(&t1, &t2);*/
    // int t = 13;
    // for (size_t i = 0; i < partition_size; i++)
    // {
    //     t = t * t;
    // }

    return;
}

void timers_init(timer **timers, TestCase *test_case, Result *result)
{
    int timer_count = 2;
    *timers = malloc(sizeof(timer) * timer_count);
    timer_init(&(*timers)[0]);
    timer_init(&(*timers)[1]);
}

void timers_start_global(timer *timers)
{
    timer_start(&timers[0]);
}
void timers_start_local(timer *timers)
{
    timer_start(&timers[1]);
}
void timers_stop_global(timer *timers)
{
    timer_stop(&timers[0]);
}
void timers_stop_local(timer *timers)
{
    timer_stop(&timers[1]);
}

void timers_free(timer *timers)
{
    free(timers);
}

void timers_store(timer *timers, Result *result)
{
    result->t_total = timers[0].sum;
    result->t_local = timers[1].sum;
    result->t_total_std_dev = timer_std_dev(&timers[0]);
    result->t_local_std_dev = timer_std_dev(&timers[1]);
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
    fprintf(file, "mode,buffer_size,partition_size,partition_size_recv,send_pattern,t_local,t_total,bandwidth,sigma(t_local),sigma(t_total)\n");
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
    fprintf(file, "%i,%lli,%lli,%lli,%s,%f,%f,%f,%f,%f\n", test_case->mode, test_case->buffer_size, test_case->partition_size, test_case->partition_size_recv, send_pattern_identifiers[test_case->send_pattern_num], result->t_local, result->t_total, result->bandwidth, result->t_local_std_dev, result->t_total_std_dev);
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
    // init send buffer with 0s and recv buffer with 1s
    if (comm_rank == 0)
    {
        init_buffer(test_case->buffer, test_case->buffer_size);
    } else {
        memset(test_case->buffer, 1, test_case->buffer_size * sizeof(char));
    }

    // call run method for this test case
    Result result;
    if (test_case->method.run != NULL)
        (*test_case->method.run)(test_case, &result, comm_rank);

    // take average duration per iteration
    result.t_total /= test_case->iteration_count;
    result.t_local /= test_case->iteration_count;

    // compute bandwidth
    result.bandwidth = ((double)test_case->buffer_size) / result.t_total;

    // check that data was transmitted correctly
    result.success = check_buffer(test_case->buffer, test_case->buffer_size);

    return result;
}

// powers of two for buffer sizes
#define B (MPI_Count)1
#define KB (MPI_Count)(1024)
#define MB (MPI_Count)(1024 * 1024)
#define GB (MPI_Count)(1024 * 1024 * 1024)

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
        return 0;

    // init test cases
    const MPI_Count buffer_size = 4 * MB;
    //                          Send = 0, Isend = 1, IsendTest = 2, IsendThenTest = 3, IsendTestall = 4, CustomPsend = 5, WinSingle = 6,            Win = 7,   Psend = 8, PsendParrived = 9, PsendProgress = 10, PsendProgressThreaded = 11, PsendThreaded = 12
    bool use_mode[ModeCount] = {    true,      true,          true,              true,             true,           false,          true,               true,        true,              true,               true,                      false,              false};

    // openmpi/5.0.0, on laptop (Ryzen 4 4700U), at 16MiB
    // Send:           tested down to     8B
    // Isend:          tested down to    64B, takes >500s, 512B takes >7s
    // IsendTest:      tested down to    64B, takes 1.6s, 32B slow(>10min, deadlock?)
    // IsendThenTest:  tested down to    64B,  (i guess, see IsendTest)
    // IsendTestall:   tested down to    64B,
    // Psend:          tested down to   128B, takes 130s
    // CustomPsend:    tested down to   1024
    // Win single:     tested down to     8B, takes 6s, now somehow fails at 1024 B
    // Win:            tested down to   512B, fails at 256B (16MiB / 64Ki) due to mmap error 12

    const MPI_Count min_partition_size[ModeCount] =
    //         Send = 0, Isend = 1, IsendTest = 2, IsendThenTest = 3, IsendTestall = 4, CustomPsend = 5, WinSingle = 6,            Win = 7,   Psend = 8, PsendParrived = 9, PsendProgress = 10, PsendProgressThreaded = 11, PsendThreaded = 12
        {           512,       512,           512,               512,              512,            4096,          2048, buffer_size / 1024,         512,               512,                512,                        512,                512 };
    const MPI_Count max_partition_size[ModeCount] =                                                                                         
    //         Send = 0, Isend = 1, IsendTest = 2, IsendThenTest = 3, IsendTestall = 4, CustomPsend = 5, WinSingle = 6,            Win = 7,   Psend = 8, PsendParrived = 9, PsendProgress = 10, PsendProgressThreaded = 11, PsendThreaded = 12
        {   buffer_size, buffer_size, buffer_size,       buffer_size,      buffer_size,     buffer_size,   buffer_size,        buffer_size, buffer_size,       buffer_size,        buffer_size,                buffer_size,        buffer_size };

    // openmpi/5.0.0, on hawk, at  64MiB
    // Send:           tested down to     1B,
    // Isend:          tested down to     1B,
    // IsendTest:      tested down to     1B,
    // IsendThenTest:  tested down to     1B,
    // IsendTestall:   tested down to     1B,
    // Psend:          tested down to     1B,
    // CustomPsend:    tested down to  2048B, at 4MiB buffer size
    // Win single:     tested down to     1B,
    // Win:            tested down to  64KiB, fails at 32KiB = 16MiB / 512 (address not mapped to object)

    // const MPI_Count min_partition_size[ModeCount] =
    // //   Send = 0      , Isend = 1    , IsendTest = 2, IsendThenTest = 3, IsendTestall = 4, Psend = 5, CustomPsend = 6, WinSingle = 7, Win = 8
    //     {             1,             1,             1,                 1,                1,         1,               1,             1, buffer_size / 256  };
    // const MPI_Count max_partition_size[ModeCount] =
    // //   Send = 0      , Isend = 1    , IsendTest = 2, IsendThenTest = 3, IsendTestall = 4,   Psend = 5, CustomPsend = 6, WinSingle = 7, Win = 8
    //     {   buffer_size,   buffer_size,   buffer_size,       buffer_size,      buffer_size, buffer_size,     buffer_size,   buffer_size, buffer_size };

    // 
    SendPattern send_patterns[] = {
        Linear,
        Stride128,
        Random,
        RandomBurst128
        //  , RandomBurst1K,
        //  LinearInverse,
        //  Stride1K
    }; 
    TestCases tests;
    test_cases_init(buffer_size, 2, use_mode, min_partition_size, max_partition_size, send_patterns, sizeof(send_patterns) / sizeof(SendPattern), &tests);

    //
    if (comm_rank == 0)
        printf("Running %i tests: \n", test_cases_get_count(tests));

    // set up result file for this rank
    FILE *result_file = open_result_file(comm_rank);
    bool success = comm_rank == 1;  // don't use success value on rank 0

    // run test cases
    for (size_t i = 0; i < test_cases_get_count(tests); i++)
    {
        TestCase *test_case = test_cases_get_test_case(tests, i);
        Result *result = test_cases_get_result(tests, i);

        if (comm_rank == 0)
        {
            if (test_case->partition_size == test_case->partition_size_recv)
                printf("Running test %.4li in mode %15s, buffer size %9lli, partition size %7lli, iteration count %3li, send pattern %i :\n\t", i, mode_names[test_case->mode], test_case->buffer_size, test_case->partition_size, test_case->iteration_count, test_case->send_pattern_num);
            else
                printf("Running test %.4li in mode %15s, buffer size %9lli, partition size %7lli -> %7lli, iteration count %3li, send pattern %i :\n\t", i, mode_names[test_case->mode], test_case->buffer_size, test_case->partition_size, test_case->partition_size_recv, test_case->iteration_count, test_case->send_pattern_num);
            fflush(stdout);
        }

        // perform warmup run when switching transfer mode as first run performs worse than the following ones
        if (i == 0 || test_case->mode != test_cases_get_test_case(tests, i - 1)->mode)
        {
            // use copy of the current test case for warmup
            TestCase warmup = *test_case;
            bench(&warmup, comm_rank, comm_size);
        }

        // if time limit was exceeded in last test, don't bench this mode any more
        double time_limit = 2.0;
        if (i > 0 && test_case->mode == test_cases_get_test_case(tests, i - 1)->mode && test_cases_get_result(tests, i - 1)->t_total * test_case->iteration_count > time_limit) {
            *result = *test_cases_get_result(tests, i-1);

            if (comm_rank == 0){
                printf("time limit exceeded in previous run, not running benchmark\n"); 
                fflush(stdout);
            }
        } else {    // otherwise, run benchmark
            *result = bench(test_case, comm_rank, comm_size);

            if (comm_rank == 0) {
                printf("success = %i, total time: %10gs, average time: %10gμs, standard deviation = %10g\n", result->success, result->t_total * test_case->iteration_count, result->t_total * 1000000, result->t_local_std_dev);
                fflush(stdout);
            }
        }

        record_result(test_case, result, result_file);
        success &= result->success;
    }

    close_result_file(result_file);
    test_cases_free(&tests);

    MPI_Finalize();

    if (comm_rank == 0)
    {
        printf("done, success: %d\n", success);
    }

    return 0;
}
