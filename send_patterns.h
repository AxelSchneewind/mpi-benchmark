#pragma once

#include "bench.h"

#include <stddef.h>


extern void make_send_pattern(unsigned int* result, size_t count, SendPattern pattern);


extern void make_linear_pattern(unsigned int *result, size_t count);
extern void make_stride_pattern(unsigned int *result, size_t count, size_t gap);
extern void make_random_pattern(unsigned int *result, size_t count);
extern void make_random_burst_pattern(unsigned int *result, size_t count, size_t burst_size);

extern void make_partition_send_pattern(const unsigned int* byte_send_pattern, unsigned int* partition_send_pattern, size_t byte_count, size_t partition_size);


