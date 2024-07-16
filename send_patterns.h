#pragma once

#include <stddef.h>

#include "permutation.h"

// identity
void make_linear_pattern(permutation result, size_t count);

// permutation that inverts order
void make_linear_reverse_pattern(permutation result, size_t count);

// basic stride pattern (marks all p[i*gap] first, then p[i*gap + 1] and so on...)
void make_stride_pattern(permutation result, size_t count, size_t gap);

// pattern that mimics the transfer of the boundary of a square grid
void make_grid_boundary_pattern(permutation result, size_t width, size_t height, permutation boundary_permutation, permutation inner_permutation);

// 
void make_random_pattern(permutation result, size_t count);
void make_random_pattern_seeded(permutation result, size_t count, int seed);

// 
void make_random_burst_pattern(permutation result, size_t count, size_t burst_size);

// 
void make_partition_send_pattern(const permutation byte_send_pattern, permutation partition_send_pattern, size_t byte_count, size_t partition_size);
