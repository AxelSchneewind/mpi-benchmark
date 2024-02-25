
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <stdbool.h>

#define minimum(a,b) (a<b) ? a : b;
#define maximum(a,b) (a>b) ? a : b;

// node representing a contiguous interval
struct node {
    struct node* parent;
    struct node* left;
    struct node* right;
    int min_marked;
    int max_marked;
};
typedef struct node node;

bool has_left(node* n) {
    return NULL != n->left;
}
bool has_right(node* n) {
    return NULL != n->right;
}

void node_init(node* n) {
    n->parent = NULL;
    n->left = NULL;
    n->right = NULL;
    n->min_marked = -1;
    n->max_marked = -1;
}

void node_set_left(node* n, node* child) {
    assert(NULL != n);
    n->left = child;
    if (NULL != child)
        child->parent = n;
}

void node_set_right(node* n, node* child) {
    assert(NULL != n);
    n->right = child;
    if (NULL != child)
        child->parent = n;
}

node* left_most_child(node* n) {
    assert(NULL != n);
    if (NULL != n->left)
        return left_most_child(n->left);
    else
        return n;
}
node* right_most_child(node* n) {
    assert(NULL != n);
    if (NULL != n->right)
        return right_most_child(n->right);
    else
        return n;
}

void expand_left(node* n) {
    assert(NULL != n);
    assert(NULL == n->left);

    node* new_node = malloc(sizeof(node));
    node_init(new_node);

    n->left = new_node;
    new_node->parent = n;
}
void expand_right(node* n) {
    assert(NULL != n);
    assert(NULL == n->right);

    node* new_node = malloc(sizeof(node));
    node_init(new_node);
    
    n->right = new_node;
    new_node->parent = n;
}

void merge_left(node* n) {
    assert(NULL != n);
    node* child = n->left;
    if (NULL == child) return;

    // next ancestor to the left
    node* right_grand_child = right_most_child(child);

    // if n and grand_child don't make up contiguous interval, no merge possible
    if (right_grand_child->max_marked != n->min_marked)
        return;

    // extend min value to min of grand_childs 
    n->min_marked = right_grand_child->min_marked;

    if (right_grand_child->parent != n) {
        node_set_right(right_grand_child->parent, right_grand_child->left);
    } else {
        node_set_left(n, NULL);
    }
    free(right_grand_child);
}
void merge_right(node* n) {
    assert(NULL != n);
    node* child = n->right;
    if (NULL == child) return;

    // next ancestor to the right
    node* left_grand_child = left_most_child(child);

    // if n and grand_child don't make up contiguous interval, no merge possible
    if(left_grand_child->min_marked != n->max_marked)
        return;

    // extend max value to max of grand_childs 
    n->max_marked = left_grand_child->max_marked;

    if (left_grand_child->parent != n) {
        node_set_left(left_grand_child->parent, left_grand_child->right);
    } else {
        node_set_right(n, NULL);
    }
    free(left_grand_child);
}

void marks_insert(node* root, int min, int max) {
    assert((NULL == root->left && NULL == root->right) || root->left != root->right);

    // node does not contain interval
    if (root->min_marked == root->max_marked) {
        assert(NULL == root->left);
        assert(NULL == root->right);
        root->min_marked = min;
        root->max_marked = max;
        return;
    }

    // given interval is directly to the left of current node
    if (max == root->min_marked) {
        root->min_marked = min;
        merge_left(root);
        return;
    }

    // given interval is directly to the right of current node
    if (min == root->max_marked) {
        root->max_marked = max;
        merge_right(root);
        return;
    }

    // given interval is to the left of current node
    if (max < root->min_marked) {
        if (NULL == root->left) {
            expand_left(root);
        }
        marks_insert(root->left, min, max);
        merge_left(root);
    } 
    
    // given interval is to the right of current node
    if (min > root->max_marked) {
        if (NULL == root->right) {
            expand_right(root);
        }
        marks_insert(root->right, min, max);
        merge_right(root);
    }
}

void node_remove(node* n) {
    if (has_left(n)) {
        n->min_marked = n->left->min_marked;
        n->max_marked = n->left->max_marked;
        node_remove(n->left);
    } else if (has_right(n)) {
        n->min_marked = n->right->min_marked;
        n->max_marked = n->right->max_marked;
        node_remove(n->right);
    } else {
        if (NULL == n->parent)
            return;
        if (n->parent->left == n) {
            node_set_left(n->parent, NULL);
            free(n);
        } else {
            node_set_right(n->parent, NULL);
            free(n);
        }
    }
} 

void marks_remove(node* root, int min, int max) {
    if (min >= max) return;

    // interval not contained
    if (root->min_marked == root->max_marked) {
        // assert(NULL == root->left);
        // assert(NULL == root->right);
        return;
    }

    if (max <= root->min_marked) {
        if (has_left(root))
            marks_remove(root->left, min, max);
        return;
    }

    if (min >= root->max_marked) {
        if (has_right(root))
            marks_remove(root->right, min, max);
        return;
    }

    // given interval is in the right of this node
    if (max == root->max_marked) {
        if (root->min_marked <= min) {
            root->max_marked = min;
        } else {
            root->max_marked = root->min_marked;
            node_remove(root);
            marks_remove(root->left, min, root->min_marked);
        }
    }

    // given interval is in the left of this node
    else if (min == root->min_marked) {
        if (root->max_marked >= max) {
            root->min_marked = max;
        } else {
            root->min_marked = root->max_marked;
            node_remove(root);
            marks_remove(root->right, root->max_marked, max);
        }
    }

    // split
    else if (max < root->max_marked) {
        marks_insert(root, root->min_marked, min);
        root->min_marked = max;
    } 
    
    else if (min > root->min_marked) {
        marks_insert(root, max, root->max_marked);
        root->max_marked = min;
    }

    if (root->min_marked == root->max_marked)
        node_remove(root);
}

void node_print(node* n, int level) {
    for (int i = 0; i < level; i++)
    {
        printf("  ");
    }

    if (NULL == n) {
        printf("x\n");
    } else {
        printf("[%i,%i]\n", n->min_marked, n->max_marked);
        if (NULL != n->left || NULL != n->right) {
            node_print(n->left, level + 1);
            node_print(n->right, level + 1);
        }
    }
}

void find_interval(node* root, int min_size, int* min, int* max) {
    *min = root->min_marked;
    *max = root->max_marked;
    if (*max - *min >= min_size)
        return;

    if (has_left(root)) {
        find_interval(root->left, min_size, min, max);
        if (*max - *min >= min_size)
            return;
    }

    if (has_right(root)) {
        find_interval(root->right, min_size, min, max);
        if (*max - *min >= min_size)
            return;
    }

    *min = 0;
    *max = -1;
}


int main() {
    node root;
    node_init(&root);

    int min_max[] = { 
        10, 11,
        11, 12,
        23, 24,
        6, 9,
        9, 10,
        68, 90,
        12, 13,
        2, 3,
        15, 18,
        90, 91,
        100, 1023,
    };
    const int num_insertions = sizeof(min_max) / sizeof(int) / 2;

    for (int i = 0; i < num_insertions; i++)
    {
        marks_insert(&root, min_max[2 * i], min_max[2 * i + 1]);
    }

    node_print(&root, 0);


    int min, max;
    int size = 10;
    while (size >= 1) {
        find_interval(&root, size, &min, &max);
        while (max - min >= size) {
            printf("found [%i,%i] (>=%i)\n", min, max, size);
            marks_remove(&root, min, max);
            node_print(&root, 0);
            find_interval(&root, size, &min, &max);
        }
        size--;
    }
    
    node_print(&root, 0);
}