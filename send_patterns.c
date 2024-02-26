#include "send_patterns.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

void make_linear_pattern(unsigned int *result, size_t count)
{
    for (size_t i = 0; i < count; i++)
    {
        result[i] = i;
    }
}

void make_linear_inverse_pattern(unsigned int *result, size_t count)
{
    for (size_t i = 0; i < count; i++)
    {
        result[i] = count - 1 - i;
    }
}

void make_stride_pattern(unsigned int *result, size_t count, size_t gap)
{
    int index = 0;
    for (size_t j = 0; j < gap; j++)
    {
        for (size_t i = j; i < count; i += gap)
        {
            result[index++] = i;
        }
    }
}

void shuffle(unsigned int *output, size_t count) {
    size_t remaining_values_count = count;
    unsigned int* remaining_values = malloc(sizeof(unsigned int) * count);
    memcpy(remaining_values, output, count * sizeof(unsigned int));

    for (size_t i = 0; i < count; i++)
    {
        size_t num = (rand() + remaining_values_count) % remaining_values_count;

        output[i] = remaining_values[num];

        remaining_values[num] = remaining_values[--remaining_values_count];
    }

    free(remaining_values);
}

void make_random_pattern(unsigned int *result, size_t count) {
    srand(0xC0FFEE);

    for (size_t i = 0; i < count; i++)
        result[i] = i;

    shuffle(result, count);
} 

void make_random_burst_pattern(unsigned int *result, size_t count, size_t burst_size) {
    srand(0xC0FFEE);

    size_t offset_count = count / burst_size;
    unsigned int* offsets = malloc(sizeof(unsigned int) * offset_count);
    for (size_t i = 0; i < offset_count; i++)
    {
        offsets[i] = i * burst_size;
    }
    shuffle(offsets, offset_count);

    for (size_t i = 0; i < offset_count; i++) {
        for (size_t j = 0; j < burst_size; j++)
        {
            result[i * burst_size + j] = offsets[i] + j;
        }
    }

    free(offsets);
}


void make_partition_send_pattern(const permutation byte_send_pattern, permutation partition_send_pattern, size_t byte_count, size_t partition_size) {
    size_t partition_count = byte_count / partition_size;
    size_t *num_ready = calloc(partition_count, sizeof(size_t));
    // memset(num_ready, 0, sizeof(size_t) * partition_count);

    size_t j = 0;
    for (size_t i = 0; i < byte_count; i++)
    {
        size_t partition_index = byte_send_pattern[i] / partition_size;
        assert(partition_index >= 0 && partition_index < partition_count);
        num_ready[partition_index] += 1;

        if (num_ready[partition_index] == partition_size) {
            assert(j >= 0 && j < partition_count);
            partition_send_pattern[j++] = partition_index;
        }
        assert(num_ready[partition_index] <= partition_size);
    }

    free(num_ready);
}

void permutation_create(permutation * out, size_t num_elements) {
    *out = calloc(num_elements, sizeof(unsigned int));
};

void permutation_destroy(permutation * out) {
    free(*out);
    *out = NULL;
};