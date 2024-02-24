
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

// node representing a contiguous interval
struct node {
    struct node* parent;
    struct node* left;
    struct node* right;
    int min;
    int max;
    int min_marked;
    int max_marked;
};
typedef struct node node;

void node_set_left(node* n, node* child) {
    assert(NULL != n);
    n->left = child;
    child->parent = n;
}

void node_set_right(node* n, node* child) {
    assert(NULL != n);
    n->right = child;
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
    new_node->min = n->min;
    new_node->max = n->min_marked;
    
    if (n->min_marked <= new_node->max) {
        new_node->min_marked = n->min_marked;
        new_node->max_marked = n->max;
    } else {
        new_node->min_marked = (new_node->max + new_node->min) / 2;
        new_node->max_marked = (new_node->max + new_node->min) / 2;
    }

    n->left = new_node;
}
void expand_right(node* n) {
    assert(NULL != n);
    assert(NULL == n->right);

    node* new_node = malloc(sizeof(node));
    new_node->max = n->max;
    new_node->min = n->max_marked;
    
    if (n->max_marked > new_node->min) {
        new_node->min_marked = new_node->min;
        new_node->max_marked = n->max_marked;
    } else {
        new_node->min_marked = (new_node->max + new_node->min) / 2;
        new_node->max_marked = (new_node->max + new_node->min) / 2;
    }

    n->right = new_node;
}

void merge_left(node* n) {
    assert(NULL != n);
    node* child = n->left;
    if (NULL == child) return;
    node* right_grand_child = right_most_child(child);

    if (right_grand_child->max != n->min || right_grand_child->max_marked != n->min_marked)
        return;

    n->min = right_grand_child->min;
    n->min_marked = right_grand_child->min_marked;

    node_set_right(right_grand_child->parent, right_grand_child->left);
    free(right_grand_child);
}
void merge_right(node* n) {
    assert(NULL != n);
    node* child = n->left;
    if (NULL == child) return;
    node* left_grand_child = left_most_child(child);

    if(n->max != left_grand_child->min || n->max_marked != left_grand_child->min_marked)
        return;

    n->max = left_grand_child->max;
    n->max_marked = left_grand_child->max_marked;

    node_set_left(left_grand_child->parent, left_grand_child->right);
    free(left_grand_child);
}

void insert(node* root, int mark) {
    assert(mark >= root->min);
    assert(mark < root->max);

    if (root->min_marked == root->max_marked) {
        assert(NULL == root->left);
        assert(NULL == root->right);
        root->min_marked = mark;
        root->max_marked = mark + 1;
        assert(root->min_marked >= root->min);
        assert(root->max_marked <= root->max);
    }

    if (mark == root->min_marked - 1) {
        root->min_marked--;
        merge_left(root);
        return;
    }

    if (mark == root->max_marked) {
        root->max_marked++;
        merge_right(root);
        return;
    }

    if (mark < root->min_marked - 1) {
        if (NULL == root->left) {
            expand_left(root);
        }
        insert(root->left, mark);
        merge_left(root);
    } 
    
    if (mark > root->max_marked) {
        if (NULL == root->right) {
            expand_right(root);
        }
        insert(root->right, mark);
        merge_right(root);
    }
}

void node_print(node* n, int level) {
    for (int i = 0; i < level; i++)
    {
        printf("  ");
    }

    if (NULL == n) {
        printf("---\n");
    } else {
        printf("[%i,%i]: [%i,%i]\n", n->min, n->max, n->min_marked, n->max_marked);
        node_print(n->left, level + 1);
        node_print(n->right, level + 1);
    }

}


int main() {
    node root;
    root.min = 0;
    root.max = 127;
    root.min_marked = (root.max + root.min) / 2;
    root.max_marked = (root.max + root.min) / 2;;

    insert(&root, 10);
    insert(&root, 11);
    insert(&root, 23);
    insert(&root, 8);
    insert(&root, 9);
    insert(&root, 68);
    insert(&root, 12);
    insert(&root, 2);
    insert(&root, 15);
    insert(&root, 90);

    node_print(&root, 0);
}