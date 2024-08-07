#include "benchmarks/bench_template.h"
#include "benchmarks/bench_psend.h"

#include <stdio.h>
#include <assert.h>



void bench_psend_progress(TestCase *test_case, Result *result, int comm_rank)
{ 
    execute(test_case, result, comm_rank, psend_test);
}

void bench_psend_parrived(TestCase *test_case, Result *result, int comm_rank)
{ 
    execute(test_case, result, comm_rank, psend_parrived);
}

void bench_psend(TestCase *test_case, Result *result, int comm_rank)
{ 
    execute(test_case, result, comm_rank, psend);
}

void bench_psend_list(TestCase *test_case, Result *result, int comm_rank)
{ 
    execute(test_case, result, comm_rank, psend_list);
}


