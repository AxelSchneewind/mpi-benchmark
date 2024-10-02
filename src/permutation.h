#pragma once

#include <stddef.h>

typedef int* permutation;

// 
void permutation_create(permutation* out, size_t num_elements);
void permutation_destroy(permutation* out);

// check if valid permutation
int permutation_is_valid(permutation p, size_t size);

// 
int permutation_is_identity(permutation p, size_t size);

// getter (so one does not have to rely on p[...], as datastructure might change)
int* permutation_at(permutation p, size_t index);

// apply perm to target (both of length len)
int permutation_apply(permutation target, permutation perm, size_t len);

