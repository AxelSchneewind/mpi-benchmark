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
    fprintf(file, "mode,buffer_size,partition_size,partition_size_recv,send_pattern,t_local,t_total,bandwidth\n");
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
    fprintf(file, "%i,%i,%i,%i,%s,%f,%f,%f\n", test_case->mode, test_case->buffer_size, test_case->partition_size, test_case->partition_size_recv, send_pattern_identifiers[test_case->send_pattern_num], result->t_local, result->t_total, result->bandwidth);

    fflush(file);
}

void close_result_file(FILE *file)
{
    fclose(file);
}

Result bench(TestCase *test_case, int comm_rank, int comm_size)
{
    // init send buffer with 0s and recv buffer with 1s
    if (comm_rank == 0)
    {
        for (size_t i = 0; i < test_case->buffer_size; i++)
        {
            test_case->buffer[i] = (char)((i * 89261) % 137);
        }
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
    bool correct = true;
    for (size_t i = 0; i < test_case->buffer_size; i++)
        if (test_case->buffer[i] != (char)((i * 89261) % 137))
            correct = false;
    result.success = correct;

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
    // MPI_Init(&argc, &argv);

    int comm_rank, comm_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &comm_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);

    // init test cases
    const MPI_Count buffer_size = 4 * MB;
    //                          Send = 0, Isend = 1, IsendTest = 2, IsendThenTest = 3, IsendTestall = 4, Psend = 5, PsendProgress = 6, PSendThreaded = 7, CustomPsend = 8, WinSingle = 9, Win = 10
  //bool use_mode[ModeCount] = {    true,      true,          true,              true,              true,     true,              true,              true,            true,           true,      true};
    bool use_mode[ModeCount] = {   false,     false,         false,             false,             false,    false,              true,             false,           false,           true,      true};

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
    //         Send = 0, Isend = 1, IsendTest = 2, IsendThenTest = 3, IsendTestall = 4,     Psend = 5, PsendProgress = 6, PSendThreaded = 7, CustomPsend = 8, WinSingle = 9, Win = 10
        {           256,       256,           256,               256,              256,           256,               256,               256,             256,          4096, buffer_size / KB  };
    const MPI_Count max_partition_size[ModeCount] =                                                                                         
    //         Send = 0, Isend = 1  , IsendTest = 2, IsendThenTest = 3, IsendTestall = 4,   Psend = 5, PsendProgress = 6, PSendThreaded = 7, CustomPsend = 8, WinSingle = 9, Win = 10
        {   buffer_size, buffer_size,   buffer_size,       buffer_size,      buffer_size, buffer_size,       buffer_size,       buffer_size,     buffer_size,   buffer_size, buffer_size };

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
        LinearInverse,
        Stride128,
        Stride1K,
        Random,
        RandomBurst128,
        RandomBurst1K
    }; 
    test_cases_init(buffer_size, 100, use_mode, min_partition_size, max_partition_size, send_patterns, sizeof(send_patterns) / sizeof(SendPattern));

    //
    if (comm_rank == 1)
        printf("Running %i tests: \n", test_cases_get_count());

    // set up result file for this rank
    FILE *result_file = open_result_file(comm_rank);
    bool success = comm_rank == 1;  // don't use success value on rank 0

    // run test cases
    for (size_t i = 0; i < test_count; i++)
    {
        TestCase *test_case = get_test_case(i);
        Result *result = get_result(i);

        if (comm_rank == 1)
        {
            if (test_case->partition_size == test_case->partition_size_recv)
                printf("Running test %.3i in mode %15s, buffer size %9i, partition size %9i, iteration count %3i, send pattern %i :\n\t", i, mode_names[test_case->mode], test_case->buffer_size, test_case->partition_size, test_case->iteration_count, test_case->send_pattern_num);
            else
                printf("Running test %.3i in mode %15s, buffer size %9i, partition size %9i -> %9i, iteration count %3i, send pattern %i :\n\t", i, mode_names[test_case->mode], test_case->buffer_size, test_case->partition_size, test_case->partition_size_recv, test_case->iteration_count, test_case->send_pattern_num);
            fflush(stdout);
        }

        // perform warmup run when switching transfer mode as first run performs worse than the following ones
        if (i == 0 || test_case->mode != get_test_case(i - 1)->mode)
        {
            // use copy of the current test case for warmup
            TestCase warmup = *test_case;
            bench(&warmup, comm_rank, comm_size);
        }

        // if time limit was exceeded in last test, don't bench this mode any more
        double time_limit = 2.0;
        if (i > 0 && test_case->mode == get_test_case(i - 1)->mode && get_result(i - 1)->t_total * test_case->iteration_count > time_limit) {
            *result = *get_result(i-1);

            if (comm_rank == 1)
                printf("time limit exceeded in previous run, not running benchmark\n");
        } else {    // otherwise, run benchmark
            *result = bench(test_case, comm_rank, comm_size);

            if (comm_rank == 1)
                printf("success = %d, average time: %10gμs, total time: %10gs\n", result->success, result->t_total * 1000000, result->t_total * test_case->iteration_count);
        }

        record_result(test_case, result, result_file);
        success &= result->success;
    }

    close_result_file(result_file);
    test_cases_free();

    MPI_Finalize();

    if (comm_rank == 1)
    {
        printf("done, success: %d\n", success);
    }

    return 0;
}