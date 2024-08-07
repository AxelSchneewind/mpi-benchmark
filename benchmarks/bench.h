#pragma once

#include "test_cases.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>

#define POST_WARMUP_SLEEP_US 5000   // pause after warmup run, helps to find actual benchmark runs in traces
#define WARMUP_ITERATIONS 10

// perform some virtual work on a partition
static void work(const MPI_Count partition_size)
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
