#pragma once

#include "bench.h"
#include "send_patterns.h"

#include <mpi.h>
#include <stdbool.h>

enum Mode
{
	Send =             0,
	Isend              ,
	IsendTest          ,
	IsendThenTest      ,
	IsendTestall       ,
	CustomPsend        ,
	WinSingle          ,
	Win                ,
	Psend              ,
	PsendParrived      ,
	PsendProgress      ,
	PsendProgressThread,
	PsendThreaded      ,
	ModeCount
};
typedef enum Mode Mode;

enum SendPattern {
	Linear = 0,
	LinearInverse,
	Stride128,
	Stride1K,
	Random,
    RandomBurst128,
    RandomBurst1K,
	SendPatternCount
};
typedef enum SendPattern SendPattern;

extern void make_send_pattern(permutation result, size_t count, SendPattern pattern);


struct TestCase;
struct Result;
typedef struct {void (*run)(struct TestCase* test_case, struct Result *result, int comm_rank); } RunMethod;

struct TestCase
{
	Mode mode;
	RunMethod method;
	char* buffer;
	enum SendPattern send_pattern_num;
	permutation send_pattern;
	permutation recv_pattern;
	size_t iteration_count;
	MPI_Count buffer_size;
	MPI_Count partition_size;
	MPI_Count partition_size_recv; 		// for now, only used by Psend
	MPI_Count partition_count;
	MPI_Count partition_count_recv;
};
typedef struct TestCase TestCase;

typedef struct Result
{
	int success;
	double t_local;			// in seconds
	double t_total;			// in seconds
	double t_total_std_dev; // standard deviation in seconds (over each run)
	double t_local_std_dev; // standard deviation in seconds (over each run)
	double bandwidth;		// in bytes per second
	MPI_Status send_status;
	MPI_Status recv_status;
};
typedef struct Result Result;


struct test_cases;
typedef struct test_cases* TestCases;

extern int test_cases_get_count(TestCases tests);
extern TestCase *test_cases_get_test_case(TestCases tests, int i);
extern Result *test_cases_get_result(TestCases tests, int i);

extern void test_cases_init(MPI_Count buffer_size, int num_repetitions, const bool *use_mode, const MPI_Count *min_partition_size, const MPI_Count *max_partition_size, const SendPattern *send_patterns, int byte_send_patterns_count, TestCases* tests);
extern void test_cases_free(TestCases* tests);
