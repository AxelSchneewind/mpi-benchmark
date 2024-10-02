#pragma once 

#include "test_cases.h"

#include "mpi.h"

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>

static const char* const send_pattern_identifiers[SendPatternCount] = {
	"linear",
    "linear inverse",
    "stride (2B)",
    "stride (128B)",
    "stride (1KB)",
    "stride (16KB)",
    "random",
    "random burst (128B)",
    "random burst (1KB)",
    "random burst (16KB)",
	"neighborhood exchange"
};


// benchmarking functions
extern void bench_send(TestCase *test_case, Result *result, int comm_rank);
extern void bench_send_persistent(TestCase *test_case, Result *result, int comm_rank);
extern void bench_send_persistent_test(TestCase *test_case, Result *result, int comm_rank);
extern void bench_isend(TestCase *test_case, Result *result, int comm_rank);
extern void bench_isend_test(TestCase *test_case, Result *result, int comm_rank);
extern void bench_isend_then_test(TestCase *test_case, Result *result, int comm_rank);
extern void bench_isend_testall(TestCase *test_case, Result *result, int comm_rank);

#ifndef DISABLE_PSEND
extern void bench_psend(TestCase *test_case, Result *result, int comm_rank);
extern void bench_psend_gpu(TestCase *test_case, Result *result, int comm_rank);
extern void bench_psend_list(TestCase *test_case, Result *result, int comm_rank);
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
	{ &bench_send_persistent },
	{ &bench_send_persistent_test },
	{ &bench_isend },
	{ &bench_isend_test },
	{ &bench_isend_then_test },
	{ &bench_isend_testall },
	{ &bench_custom_psend },
	{ &bench_win_single },
	{ &bench_win },
#ifndef DISABLE_PSEND
	{ &bench_psend },
	{ &bench_psend_gpu },
	{ &bench_psend_list },
	{ &bench_psend_parrived },
	{ &bench_psend_progress },
	{ &bench_psend_progress_thread },
#else 
	{ &bench_send },
	{ &bench_send },
	{ &bench_send },
	{ &bench_send },
	{ &bench_send },
	{ &bench_send }
#endif
};


static const char* const mode_names[ModeCount] = {
	"Send",
	"SendPersistent",
	"SendPersistentTest",
	"Isend",
	"IsendTest",
	"IsendThenTest",
	"IsendTestall",
	"PsendCustom",
	"WinSingle",
	"Win",
	"Psend",
	"PsendOffloaded",
	"PsendList",
	"PsendParrived",
	"PsendProgress",
	"PsendProgressThreaded",
};



Result bench(TestCase *test_case, int comm_rank, int comm_size);

