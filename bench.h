#pragma once 

#include "timers.h"
#include "send_patterns.h"
#include "test_cases.h"

#include "mpi.h"
#include "stdlib.h"
#include <stdio.h>
#include <time.h>

static const char* const send_pattern_identifiers[SendPatternCount] = {
    "linear",
    "linear inverse",
    "stride (128B)",
    "stride (1KB)",
    "stride (16KB)",
    "random",
    "random burst (128B)",
    "random burst (1KB)",
    "random burst (16KB)"
};

extern void bench_send(TestCase *test_case, Result *result, int comm_rank);
extern void bench_send_persistent(TestCase *test_case, Result *result, int comm_rank);
extern void bench_isend(TestCase *test_case, Result *result, int comm_rank);
extern void bench_isend_threaded(TestCase *test_case, Result *result, int comm_rank);
extern void bench_isend_test(TestCase *test_case, Result *result, int comm_rank);
extern void bench_isend_then_test(TestCase *test_case, Result *result, int comm_rank);
extern void bench_isend_testall(TestCase *test_case, Result *result, int comm_rank);

#ifndef DISABLE_PSEND
extern void bench_psend(TestCase *test_case, Result *result, int comm_rank);
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
	{ &bench_isend },
	{ &bench_isend_threaded },
	{ &bench_isend_test },
	{ &bench_isend_then_test },
	{ &bench_isend_testall },
	{ &bench_custom_psend },
	{ &bench_win_single },
	{ &bench_win },
#ifndef DISABLE_PSEND
	{ &bench_psend },
	{ &bench_psend_list },
	{ &bench_psend_parrived },
	{ &bench_psend_progress },
	{ &bench_psend_progress_thread },
	{ &bench_psend_threaded }
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
	"Isend",
	"IsendThreaded",
	"IsendTest",
	"IsendThenTest",
	"IsendTestall",
	"PsendCustom",
	"WinSingle",
	"Win",
	"Psend",
	"PsendList",
	"PsendParrived",
	"PsendProgress",
	"PsendProgressThreaded",
	"PsendThreaded"
};



extern void work(const MPI_Count partition_size);

extern Result bench(TestCase *test_case, int comm_rank, int comm_size);



// macro for checking that mpi call was successfull
#define MPI_CHECK(x)                                                           \
  do {                                                                         \
    int __ret = (x);                                                           \
    if (MPI_SUCCESS != __ret) {                                                \
      char err_string[MPI_MAX_ERROR_STRING];                                   \
      int err_string_len = 0;                                                  \
      MPI_Error_string(__ret, err_string, &err_string_len);                    \
      fprintf(stderr, "(%s:%d) ERROR: MPI call returned error code %d (%s)",   \
              __FILE__, __LINE__, __ret, err_string);                          \
    }                                                                          \
  } while (0)
