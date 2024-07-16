#include "permutation.h"

#include <stdlib.h>
#include <string.h>


int* permutation_at(int* p, size_t index) {
    if (NULL == p)
        return NULL;

    return &p[index];
}

int permutation_is_identity(int* p, size_t size) 
{
    if (NULL == p || !permutation_is_valid(p, size))
        return 0;

    for (size_t i = 0; i < size; i++)
    {
        if (p[i] != i)
            return 0;
    }

    return 1;
};

// TODO check that each entry only appears once
int permutation_is_valid(int* p, size_t size) {
    char* const number_found = calloc(sizeof(int), size);
    memset(number_found, 0, size * sizeof(int));

    for (int i = 0; i < size; i++) {
        if (p[i] < size && p[i] >= 0 && number_found[p[i]] == 0)
            number_found[p[i]]++;
        else
            return 0;
    }

    for (int i = 0; i < size; i++) {
        if (1 != number_found[i])
            return 0;
    }

    return 1;
};

int permutation_apply(int* target, int* perm, size_t len)
{
    if (NULL == target || NULL == perm) {
        return 0;           // failure
    }

    permutation temp;
    permutation_create(&temp, len);

    // write to temporary permutation
    for (size_t i = 0; i < len; i++)
    {
        temp[i] = target[perm[i]];
    }

    // write back to target
    for (size_t i = 0; i < len; i++)
    {
        target[i] = temp[i];
    }

    permutation_destroy(&temp);

    return permutation_is_valid(target, len);
};

int permutation_invert(int* target, size_t len)
{
    if (NULL == target) {
        return 0;               // failure
    }

    permutation temp;
    permutation_create(&temp, len);

    // write to temporary permutation
    for (size_t i = 0; i < len; i++)
    {
        temp[target[i]] = i;
    }

    // write back to target
    for (size_t i = 0; i < len; i++)
    {
        target[i] = temp[i];
    }

    permutation_destroy(&temp);

    return permutation_is_valid(target, len);
};



