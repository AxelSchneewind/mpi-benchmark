#pragma once

#include "bench.h"
#include "send_patterns.h"
#include "timers.h"

#include <mpi.h>
#include <stdbool.h>

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
	Isend               ,
	IsendTest           ,
	IsendThenTest       ,
	IsendTestall        ,
	CustomPsend         ,
	WinSingle           ,
	Win                 ,
	Psend               ,
	PsendList           ,
	PsendParrived       ,
	PsendProgress       ,
	PsendProgressThread ,
	ModeCount
};
typedef enum Mode Mode;

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

int send_pattern_partition_dependent(SendPattern pattern);

struct TestCase;
struct Result;
typedef struct { void (*run)(struct TestCase* test_case, struct Result *result, int comm_rank); } RunMethod;

// 
struct TestCase
{
	Mode mode;
	RunMethod method;
	char* buffer;
	enum SendPattern send_pattern_num;
	permutation send_pattern;
	permutation recv_pattern;
	int thread_count;
	int partitions_per_thread;
	size_t iteration_count;
	MPI_Count buffer_size;
	MPI_Count partition_size;
	MPI_Count partition_size_recv; 		// only used by Psend
	MPI_Count partition_count;
	MPI_Count partition_count_recv;
};
typedef struct TestCase TestCase;

struct Result
{
	int success;
    double timings[TimerCount];             // in seconds
    double timings_std_dev[TimerCount];     // standard deviation in seconds (over each run)
	double bandwidth;                       // in bytes per second
	MPI_Status send_status;
	MPI_Status recv_status;
};
typedef struct Result Result;

// properties of the different modes
int is_psend(Mode mode);
int is_threaded(Mode mode);

// 
extern void timers_store(timers timers, Result* result);

// forward declaration for test cases struct
struct test_cases;
typedef struct test_cases* TestCases;

extern int test_cases_get_count(TestCases tests);
extern TestCase *test_cases_get_test_case(TestCases tests, int i);
extern Result *test_cases_get_result(TestCases tests, int i);

// forward declaration for setup struct
struct setup_t;
typedef struct setup_t* setup;

extern void test_cases_init(setup setup, TestCases* tests);
extern void test_cases_free(TestCases* tests);
