#include "interval_tree.h"

int main() {
    tree root;
    tree_create(&root);

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
