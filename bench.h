#pragma once 

#include "timer.h"

#include "mpi.h"
#include "stdlib.h"


typedef enum
{
	Send =             0,
	Isend              ,
	IsendTest          ,
	IsendThenTest      ,
	IsendTestall       ,
	CustomPsend        ,
	WinSingle          ,
	Win                ,
#ifndef DISABLE_PSEND
	Psend              ,
	PsendParrived      ,
	PsendProgress      ,
	PsendProgressThread,
	PsendThreaded      ,
#endif
	ModeCount
} Mode;

typedef struct
{
	int success;
	double t_local;			// in seconds
	double t_total;			// in seconds
	double t_total_std_dev; // standard deviation in seconds (over each run)
	double t_local_std_dev; // standard deviation in seconds (over each run)
	double bandwidth;		// in bytes per second
	MPI_Status send_status;
	MPI_Status recv_status;
} Result;

struct TestCase;
typedef struct {void (*run)(struct TestCase* test_case, Result *result, int comm_rank); } RunMethod;

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

static const char* const send_pattern_identifiers[SendPatternCount] = {
    "linear",
    "linear inverse",
    "stride (128B)",
    "stride (1KB)",
    "random",
    "random burst (128B at a time)",
    "random burst (1KB at a time)"
};




struct TestCase
{
	Mode mode;
	RunMethod method;
	char* buffer;
	enum SendPattern send_pattern_num;
	unsigned int* send_pattern;
	size_t iteration_count;
	MPI_Count buffer_size;
	MPI_Count partition_size;
	MPI_Count partition_size_recv; 		// for now, only used by Psend
	MPI_Count partition_count;
	MPI_Count partition_count_recv;
};
typedef struct TestCase TestCase;


extern void bench_send(TestCase *test_case, Result *result, int comm_rank);
extern void bench_isend(TestCase *test_case, Result *result, int comm_rank);
extern void bench_isend_test(TestCase *test_case, Result *result, int comm_rank);
extern void bench_isend_then_test(TestCase *test_case, Result *result, int comm_rank);
extern void bench_isend_testall(TestCase *test_case, Result *result, int comm_rank);

#ifndef DISABLE_PSEND
extern void bench_psend(TestCase *test_case, Result *result, int comm_rank);
extern void bench_psend_parrived(TestCase *test_case, Result *result, int comm_rank);
extern void bench_psend_progress(TestCase *test_case, Result *result, int comm_rank);
extern void bench_psend_progress_thread(TestCase *test_case, Result *result, int comm_rank);
extern void bench_psend_threaded(TestCase *test_case, Result *result, int comm_rank);
#endif

extern void bench_custom_psend(TestCase *test_case, Result *result, int comm_rank);
extern void bench_win_single(TestCase *test_case, Result *result, int comm_rank);
extern void bench_win(TestCase *test_case, Result *result, int comm_rank);

static RunMethod const mode_methods[ModeCount] = {
	{ &bench_send },
	{ &bench_isend },
	{ &bench_isend_test },
	{ &bench_isend_then_test },
	{ &bench_isend_testall },
	{ &bench_custom_psend },
	{ &bench_win_single },
	{ &bench_win }
#ifndef DISABLE_PSEND
	, 
	{ &bench_psend },
	{ &bench_psend_parrived },
	{ &bench_psend_progress },
	{ &bench_psend_progress_thread },
	{ &bench_psend_threaded }
#endif
};

static const char* const mode_names[ModeCount] = {
	"Send",
	"Isend",
	"IsendTest",
	"IsendThenTest",
	"IsendTestall",
	"Psendcustom",
	"WinSingle",
	"Win"
#ifndef DISABLE_PSEND
	,
	"Psend",
	"PsendParrived",
	"PsendProgress",
	"PsendProgressThreaded",
	"PsendThreaded"
#endif
};


#include <time.h>

extern void work(const MPI_Count partition_size);

extern void timers_init(timer** timers, TestCase* test_case, Result* result);
extern void timers_start_global(timer* timers);
extern void timers_start_local(timer* timers);
extern void timers_stop_global(timer* timers);
extern void timers_stop_local(timer* timers);
extern void timers_free(timer* timers);
extern void timers_store(timer* timers, Result* result);

extern Result bench(TestCase *test_case, int comm_rank, int comm_size);