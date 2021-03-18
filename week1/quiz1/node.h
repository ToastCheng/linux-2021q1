typedef struct __node {                   
    int value;
    struct __node *next;
} node_t;

static inline void list_free(node_t **list) {
    while (*list) {
        node_t* tmp = *list;
        *list = (*list)->next;
        free(tmp);
    }
}

static inline node_t* list_make_node_t(node_t *list, int val) {
    node_t* n;
    n = malloc(sizeof(node_t));
    n->value = val;

    node_t **ll = &list;
    while (*ll) {
        ll = &((*ll)->next);
    }
    *ll = n;
    return list;
}

static bool list_is_ordered(node_t *list) {
    bool first = true;
    int value;
    while (list) {
        if (first) {
            value = list->value;
            first = false;
        } else {
            if (list->value < value)
                return false;
            value = list->value;
        }
        list = list->next;
    }
    return true;
}

static void list_display(node_t *list) {
    printf("%s IN ORDER : ", list_is_ordered(list) ? "   " : "NOT");
    while (list) {
        printf("%d ", list->value);
        list = list->next;
    }
    printf("\n");
}