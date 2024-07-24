#include "send_patterns.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>



/********** PATTERN CREATION ***********/

void make_linear_pattern(int *result, size_t count)
{
    for (size_t i = 0; i < count; i++)
    {
        result[i] = i;
    }

    assert(permutation_is_valid(result, count));
}

void make_linear_reverse_pattern(int *result, size_t count)
{
    for (size_t i = 0; i < count; i++)
    {
        result[i] = count - 1 - i;
    }

    assert(permutation_is_valid(result, count));
}

void make_stride_pattern(int *result, size_t count, size_t gap)
{
    int index = 0;
    for (size_t j = 0; j < gap; j++)
    {
        for (size_t i = j; i < count; i += gap)
        {
            result[index++] = i;
        }
    }

    assert(permutation_is_valid(result, count));
}

// 
void make_grid_boundary_pattern(int* result, size_t width, size_t height, int* boundary_permutation, int* inner_permutation) 
{
    int numbers_put = 0;

    // insert topmost row
    for (size_t i = 0; i < width; i++) {
        result[numbers_put++] = i;
    }

    // insert leftmost and rightmost cell of each row
    if (height > 2) {
        for (size_t i = 1; i < height - 1; i++)
        {
            result[numbers_put++] = i * width;
            result[numbers_put++] = (i + 1) * width - 1;
        }

    }

    // insert last row
    if (height > 1) {
        for (size_t i = 0; i < width; i++) {
            result[numbers_put++] = (width * (height - 1)) + i;
        }
    }

    int boundary_size = numbers_put;
    assert(boundary_size <= width * height);

    // permute boundary cells
    if (NULL != boundary_permutation)
        permutation_apply(result, boundary_permutation, numbers_put);
    
    // insert inner entries
    if (height > 3 && width > 3) {
        size_t inner_index = 0;
        while(inner_index < (width - 2) * (height - 2)) {
            size_t i = inner_index++;

            // coordinates in inner grid
            size_t x = i % (width - 2);
            size_t y = i / (width - 2);

            // translate to index in entire grid
            size_t global_index = (y + 1) * width + (x + 1);

            result[numbers_put++] = global_index;
        }
    }

    assert(numbers_put <= width * height);

    // permute inner grid cells
    if (NULL != inner_permutation)
        permutation_apply(result + boundary_size, inner_permutation, numbers_put - boundary_size);
    assert(permutation_is_valid(result, width * height));
};

// 
void shuffle(int *output, size_t count) {
    size_t remaining_values_count = count;
    int* remaining_values = malloc(sizeof(int) * count);
    memcpy(remaining_values, output, count * sizeof(int));

    for (size_t i = 0; i < count; i++)
    {
        size_t num = (rand() + remaining_values_count) % remaining_values_count;

        output[i] = remaining_values[num];

        remaining_values[num] = remaining_values[--remaining_values_count];
    }

    free(remaining_values);
};

void make_random_pattern(int *result, size_t count) {
    srand(time(NULL));
    for (size_t i = 0; i < count; i++)
        result[i] = i;

    shuffle(result, count);
};

void make_random_pattern_seeded(int *result, size_t count, int seed) {
    srand(seed);

    for (size_t i = 0; i < count; i++)
        result[i] = i;

    shuffle(result, count);
} ;


void make_random_burst_pattern(int *result, size_t count, size_t burst_size) {
    srand(0xC0FFEE);

    size_t offset_count = count / burst_size;
    int* offsets = malloc(sizeof(int) * offset_count);
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
};


void make_partition_send_pattern(const permutation byte_send_pattern, permutation partition_send_pattern, size_t byte_count, size_t partition_size) {
    int partition_count = byte_count / partition_size;
    int *num_ready = calloc(partition_count, sizeof(int));
    memset(num_ready, 0, sizeof(int) * partition_count);

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
};
