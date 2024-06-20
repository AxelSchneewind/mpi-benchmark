
typedef int index_type;

struct interval {
    index_type left;
    index_type right;
};

int interval_is_empty(struct interval* i);

struct interval_list {
    index_type* left;
    index_type* right;

    int count;
};

void interval_list_reset(struct interval_list* list);

int left_index_in_range(struct interval_list* list, index_type index);
int right_index_in_range(struct interval_list* list, index_type index);
int interval_in_range(struct interval_list* list, struct interval interval);

void interval_list_init(struct interval_list* list, index_type count);
void interval_list_free(struct interval_list* list);

void interval_list_insert(struct interval_list* list, index_type number);
int interval_list_insert_list(struct interval_list* list, index_type* numbers, index_type count);

// TODO hide
struct interval interval_list_pull_largest(struct interval_list* list);
struct interval interval_list_pull_first(struct interval_list* list, index_type size);

struct selection_strategy;
struct interval interval_list_pull(struct interval_list* list, struct selection_strategy* strategy);
// TODO implement
int interval_list_pull_list(struct interval_list* list, struct selection_strategy* strategy, index_type* out);

void print_interval_list(struct interval_list* list);
void print_list(int* numbers, int count);

