#include "benchmarks/bench_template.h"
#include "benchmarks/bench_psend.h"


#include <stdio.h>
#include <assert.h>

void bench_psend_gpu_progress(TestCase *test_case, Result *result, int comm_rank)
{ 
    execute_gpu(test_case, result, comm_rank, psend_test);
}

void bench_psend_gpu_parrived(TestCase *test_case, Result *result, int comm_rank)
{ 
    execute_gpu(test_case, result, comm_rank, psend_parrived);
}

void bench_psend_gpu(TestCase *test_case, Result *result, int comm_rank)
{ 
    execute_gpu(test_case, result, comm_rank, psend);
}


