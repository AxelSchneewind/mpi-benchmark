#pragma once

#include <stddef.h>

typedef unsigned int* permutation;

extern void permutation_create(permutation * out, size_t num_elements);
extern void permutation_destroy(permutation * out);

extern void make_linear_pattern(permutation result, size_t count);
extern void make_linear_inverse_pattern(permutation result, size_t count);
extern void make_stride_pattern(permutation result, size_t count, size_t gap);
extern void make_random_pattern(permutation result, size_t count);
extern void make_random_burst_pattern(permutation result, size_t count, size_t burst_size);

extern void make_partition_send_pattern(const permutation byte_send_pattern, permutation partition_send_pattern, size_t byte_count, size_t partition_size);


