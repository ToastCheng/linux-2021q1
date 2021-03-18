#include <string.h>

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "list.h"

typedef struct __element {
    char *value;
    struct list_head list;
} list_ele_t;

static list_ele_t *get_middle(struct list_head *list)
{
    struct list_head *fast = list->next, *slow;
    list_for_each (slow, list) {
        if (fast->next == list || fast->next->next == list)
            break;
        fast = fast->next->next;
    }
    return list_entry(/* TTT */ slow, list_ele_t, list);
}

static void list_merge(struct list_head *lhs,
                       struct list_head *rhs,
                       struct list_head *head)
{
    INIT_LIST_HEAD(head);
    if (list_empty(lhs)) {
        list_splice_tail(lhs, head);
        return;
    }
    if (list_empty(rhs)) {
        list_splice_tail(rhs, head);
        return;
    }

    while (!list_empty(lhs) && !list_empty(rhs)) {
        char *lv = list_entry(lhs->next, list_ele_t, list)->value;
        char *rv = list_entry(rhs->next, list_ele_t, list)->value;
        struct list_head *tmp = strcmp(lv, rv) <= 0 ? lhs->next : rhs->next;
        list_del(tmp);
        list_add_tail(tmp, head);
    }
    list_splice_tail(list_empty(lhs) ? rhs : lhs, head);
}

void list_merge_sort(struct list_head *head)
{
    if (list_is_singular(head))
        return;

    struct list_head *left = malloc(sizeof(struct list_head));
    struct list_head sorted;
    INIT_LIST_HEAD(left);
    list_cut_position(left, head, /* MMM */  &get_middle(head)->list);
    list_merge_sort(left);
    list_merge_sort(head);
    list_merge(left, head, &sorted);
    INIT_LIST_HEAD(head);
    list_splice_tail(&sorted, head);
    free(left);
}


static bool validate(struct list_head *head)
{
    struct list_head *node;
    list_for_each (node, head) {
        if (node->next == head)
            break;
        if (strcmp(list_entry(node, list_ele_t, list)->value,
                   list_entry(node->next, list_ele_t, list)->value) > 0)
            return false;
    }
    return true;
}

bool list_insert_tail(struct list_head **head, char *s)
{
    if (!*head) return false;

    list_ele_t *newh = malloc(sizeof(list_ele_t));
    if (!newh)
        return false;

    char *new_value = strdup(s);
    if (!new_value) {
        free(newh);
        return false;
    }

    LIST_HEAD(lh);

    newh->value = new_value;
    newh->list = lh;
    list_add_tail(&newh->list, *head);

    return true;
}

void ele_free(list_ele_t *ele) {
    // char *ss = strdup(ele->value);
    free(ele->value);
    free(ele);

    // printf("freed: %s", ss);
    // free(ss);
}



void display(struct list_head *head) {
    printf("# display: \n");
    struct list_head *node;
    list_for_each (node, head) {
        printf("%s", list_entry(node, list_ele_t, list)->value);
    }
    printf("\n");
}

int main(void)
{
    FILE *fp = fopen("cities.txt", "r");
    if (!fp) {
        perror("failed to open cities.txt");
        exit(EXIT_FAILURE);
    }

    LIST_HEAD(h);
    struct list_head *head = &h;
    INIT_LIST_HEAD(head);
    char buf[256];
    while (fgets(buf, 256, fp)) {
        list_insert_tail(&head, buf);
    }
    fclose(fp);

    printf("# start sorting..\n");

    list_merge_sort(head);
    assert(validate(head));

    list_free(head, list_ele_t, list, ele_free);
    // printf("ee: %d\n", list_entry(head, list_ele_t, list));
    // struct list_head *it = NULL;
    // list_for_each(it, head) {
    //     ele_free(list_entry(it, list_ele_t, list));
    // }
    return 0;
}