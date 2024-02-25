struct node;
typedef struct node* tree;

void marks_insert(tree root, int min, int max);
void node_remove(tree n);
void marks_remove(tree root, int min, int max);
void node_print(tree n, int level);
void find_interval(tree root, int min_size, int* min, int* max);

void tree_create(tree* t);
