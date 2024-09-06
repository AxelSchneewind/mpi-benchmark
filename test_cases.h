#pragma once

#include "send_patterns.h"
#include "timers.h"

#include <mpi.h>

// different timers
enum Timer {
    Total = 0,
    Iteration,
    IterationStartToWait,
    IterationWait,
    TimerCount
};
typedef enum Timer Timer;

// different sending modes
enum Mode
{
    Send =             0,
    SendPersistent      ,
    SendPersistentTest  ,
    Isend               ,
    IsendTest           ,
    IsendThenTest       ,
    IsendTestall        ,
    CustomPsend         ,
    WinSingle           ,
    Win                 ,
    Psend               ,
    PsendOffloaded      ,
    PsendList           ,
    PsendParrived       ,
    PsendProgress       ,
    PsendProgressThread ,
    ModeCount
};
typedef enum Mode Mode;

// properties of the different modes
int is_psend(Mode mode);



// different send patterns
enum SendPattern {
    Linear = 0,
    LinearInverse,
    Stride2,
    Stride128,
    Stride1K,
    Stride16K,
    Random,
    RandomBurst128,
    RandomBurst1K,
    RandomBurst16K,
    GridBoundary,
    SendPatternCount
};
typedef enum SendPattern SendPattern;


// properties of send patterns
int send_pattern_partition_dependent(SendPattern pattern);

// forward declarations
struct TestCase;
struct Result;

// function pointer for benchmark entry point
typedef struct { void (*run)(struct TestCase* test_case, struct Result *result, int comm_rank); } RunMethod;

// struct defining a single benchmark run
struct TestCase
{
    // identifiers (i.e. benchmark name and run number)
    const char* name;
    int number;

    // send mechanism
    Mode mode;

    // entry point
    RunMethod method;

    // data buffer
    char* buffer;
    MPI_Count buffer_size;

    enum SendPattern send_pattern_num;
    permutation send_pattern;
    permutation recv_pattern;

    int iteration_count;

    int warmup_iterations;

    // time to sleep after warmup (in microseconds), to help find benchmark runs in traces
    int post_warmup_sleep;

    int thread_count;

    // partitioning
    int partitions_per_thread;          // computed from partitioning and number of threads, for current rank

    // send side partitioning
    MPI_Count partition_size;
    MPI_Count partition_count;

    // recv side partitioning, only used by Psend
    MPI_Count partition_size_recv;
    MPI_Count partition_count_recv;
};
typedef struct TestCase TestCase;

// struct to store benchmark results into
struct Result
{
    // whether the benchmark run succeeded (1 if sucessful, else 0)
    int success;

    // time for different parts of transfer in seconds
    double timings[TimerCount];

    // standard deviation of times in seconds (over the iterations)
    double timings_std_dev[TimerCount];
    
    // effective bandwidth in bytes per second
    double bandwidth;

    // statuses of send and recv operations (only for transfers consisting of a single request)
    MPI_Status send_status;
    MPI_Status recv_status;
};
typedef struct Result Result;

// stores the list of times into a result object
void timers_store(timers timers, Result* result);

// forward declaration for test cases struct
struct test_cases;
typedef struct test_cases* TestCases;

int test_cases_get_count(TestCases tests);
TestCase *test_cases_get_test_case(TestCases tests, int i);
Result *test_cases_get_result(TestCases tests, int i);

// forward declaration for configuration struct
struct setup_t;
typedef struct setup_t* setup;

/// computes a list of test cases from a given configuration
void test_cases_init(setup setup, TestCases* tests);
void test_cases_free(TestCases* tests);
