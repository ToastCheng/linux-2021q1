#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <linux/random.h>

#include "node.h"
#include "squares.h"

#define two32 4294967296.
#define k 0x548c9decbce65297

static inline void list_add_node_t(node_t **list, node_t *node_t) {
    node_t->next = *list;
    *list = node_t;
}

static inline void list_concat(node_t **left, node_t *right) {
    while (*left)
        left = &((*left)->next);
    *left = right;
}

void quicksort(node_t **list)
{
    if (!*list)
        return;

    node_t *pivot = *list;
    int value = pivot->value;
    node_t *p = pivot->next;
    pivot->next = NULL;

    node_t *left = NULL, *right = NULL;
    while (p) {
        node_t *n = p;
        p = p->next;
        list_add_node_t(n->value > value ? &right : &left, n);
    }

    quicksort(&left);
    quicksort(&right);

    node_t *result = NULL;
    list_concat(&result, left);
    list_concat(&result, pivot); list_concat(&result, right);
    *list = result;
}





int main(int argc, char **argv) {
    uint64_t *seed = malloc(sizeof(uint64_t));
    syscall(SYS_getrandom, seed, 8, 0);
    size_t count = 20;

    node_t *list = NULL;
    while (count--) {
        list = list_make_node_t(list, squares((*seed)++,k) % 1024);
    }
    list_display(list);
    quicksort(&list);
    list_display(list);

    if (!list_is_ordered(list))
        return EXIT_FAILURE;

    list_free(&list);
    return EXIT_SUCCESS;
}
