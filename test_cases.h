#include "bench.h"
#include "send_patterns.h"

#include <stdbool.h>

struct test_cases;
typedef struct test_cases* TestCases;

extern int test_cases_get_count(TestCases tests);
extern TestCase *test_cases_get_test_case(TestCases tests, int i);
extern Result *test_cases_get_result(TestCases tests, int i);

extern void test_cases_init(MPI_Count buffer_size, int num_repetitions, const bool *use_mode, const MPI_Count *min_partition_size, const MPI_Count *max_partition_size, const SendPattern *send_patterns, int byte_send_patterns_count, TestCases* tests);
extern void test_cases_free(TestCases* tests);
