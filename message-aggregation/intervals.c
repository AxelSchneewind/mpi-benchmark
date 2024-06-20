#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>

#include "intervals.h"
#include "send_patterns.h"

typedef int index_type;

int interval_is_empty(struct interval* i) {
    return i->left == i->right;
}


void interval_list_reset(struct interval_list* list) {
    if (NULL != list->left)
        memset(list->left, -1, list->count * sizeof(index_type));
    if (NULL != list->right)
        memset(list->right, -1, list->count * sizeof(index_type));
}


int left_index_in_range(struct interval_list* list, index_type index) {
    return index >= 0 && index < list->count;
}

int right_index_in_range(struct interval_list* list, index_type index) {
    return index >= 0 && index <= list->count;
}

int interval_in_range(struct interval_list* list, struct interval interval) {
    return interval.left >= 0 && interval.left < list->count && 
           interval.right >= 0 && interval.right <= list->count; 
}

void interval_list_init(struct interval_list* list, index_type count) {
    list->count = count;
    list->left  = calloc(sizeof(index_type), count);
    list->right = calloc(sizeof(index_type), count);

    memset(list->left, -1, count * sizeof(index_type));
    memset(list->right, -1, count * sizeof(index_type));
}

void interval_list_free(struct interval_list* list) {
    list->count = 0;
    free(list->left);
    free(list->right);
}

void interval_list_insert(struct interval_list* list, index_type number) {
    assert(-1 == list->left[number]);
    assert(-1 == list->right[number]);

    // TODO reduce number of necessary updates
    struct interval current_interval;

    // get left bound from previous entry
    if (number > 0 && list->left[number - 1] >= 0)
        current_interval.left = list->left[number - 1];
    else 
        current_interval.left = number;

    // get right bound from next entry
    if (number + 1 < list->count && list->right[number + 1] >= 0)
        current_interval.right = list->right[number + 1];
    else 
        current_interval.right = number + 1;

    assert(left_index_in_range(list, current_interval.left));
    assert(right_index_in_range(list, current_interval.right));

    // update leftmost entry of the current interval
    list->right[current_interval.left] = current_interval.right;

    // update rightmost entry of the current interval
    list->left[current_interval.right - 1] = current_interval.left;

    // update entry at this index
    list->left[number] = current_interval.left;
    list->right[number] = current_interval.right;       // optional?
}

int interval_list_insert_list(struct interval_list* list, index_type* numbers, index_type count) {
    for (index_type i = 0; i < count; i++) {
        interval_list_insert(list, numbers[i]);
    }
    return count;
}


struct interval interval_list_pull_largest_min(struct interval_list* list, index_type min_size) {
    index_type index = 0;
    // find largest interval
    for (int i = 0; i < list->count; i++)
        if (list->right[i] - list->left[i] > list->right[index] - list->left[index])
            index = i;

    // 
    struct interval result = { -1, -1 };
    if (list->right[index] - list->left[index] < min_size) {
        return result;
    } else {
        result.left  = list->left[index];
        result.right = list->right[index];
    }

    // reset counters
    for (int i = result.left; i < result.right; i++) {
        list->left[i] = -1;
        list->right[i] = -1;
    }

    return result;
}

struct interval interval_list_pull_largest(struct interval_list* list) {
    index_type index = 0;
    // find largest interval
    for (int i = 0; i < list->count; i++)
        if (list->right[i] - list->left[i] > list->right[index] - list->left[index])
            index = i;

    // 
    struct interval result = { list->left[index], list->right[index] };

    // reset counters
    for (int i = result.left; i < result.right; i++) {
        list->left[i] = -1;
        list->right[i] = -1;
    }

    return result;
}

struct interval interval_list_pull_first(struct interval_list* list, index_type size) {
    assert(size >= 0 && size <= list->count);

    struct interval result = { -1, -1 };
    for (int i = 0; i < list->count; i++){
        if (list->right[i] - list->left[i] >= size) {
            result.left = list->left[i];
            result.right = list->right[i];
            assert(interval_in_range(list, result));

            // reset counters
            for (int j = result.left; j < result.right; j++){
                list->left[j] = -1;
                list->right[j] = -1;
            }

            return result;
        } else if (list->right[i] > i) { // jump to end of this interval
            i = list->right[i];
        }
    }

    return result;
}


enum selection_mode {
    LARGEST,
    LARGEST_AT_LEAST,
    LARGEST_MULTIPLE,
    FIRST,
    FIRST_AT_LEAST
};

union selection_parameters {
    struct { } largest;
    struct { int min_size; } largest_at_least;
    struct { int factor; } largest_multiple;
    struct { } first;
    struct { int min_size; } first_at_least;
};

struct selection_strategy {
    enum selection_mode mode;
    union selection_parameters parameters;
};

struct selection_strategy* selection_strategy_create(enum selection_mode mode) {
    struct selection_strategy* result = malloc(sizeof(struct selection_strategy));
    result->mode = mode;
    memset(&result->parameters, 0, sizeof(union selection_parameters));
    return result;
};

struct interval interval_list_pull(struct interval_list* list, struct selection_strategy* strategy) {
    switch (strategy->mode) {
        case LARGEST: 
            return interval_list_pull_largest(list);
        case LARGEST_AT_LEAST: 
            return interval_list_pull_largest_min(list, strategy->parameters.largest_at_least.min_size);
        case LARGEST_MULTIPLE: 
            // TODO implement
            return interval_list_pull_largest_min(list, strategy->parameters.largest_multiple.factor);
        case FIRST: 
            return interval_list_pull_first(list, 1);
        case FIRST_AT_LEAST:
            return interval_list_pull_first(list, strategy->parameters.first_at_least.min_size);
    }
    struct interval result = {-1,-1};
    return result;
}

int interval_list_pull_list(struct interval_list* list, struct selection_strategy* strategy, index_type* out) {
    struct interval result = interval_list_pull(list, strategy);

    int count = 0;
    for (int j = result.left; j < result.right; j++){
        out[count++] = j;
    }

    return count;
}

void print_interval_list(struct interval_list* list) {
    for (int i = 0; i < list->count; i++){
        printf("[%i,%i], ", list->left[i], list->right[i]);
    }
    printf("\b\b\n");
}

void print_list(int* numbers, int count) {
    for (int i = 0; i < count; i++){
        printf("%i, ", numbers[i]);
    }
    printf("\b\b\n");
}

