#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "harness.h"
#include "queue.h"

/*
 * Create empty queue.
 * Return NULL if could not allocate space.
 */
queue_t *q_new()
{
    queue_t *q = malloc(sizeof(queue_t));
    if (q) {
        q->head = NULL;
        q->tail = NULL;
        q->size = 0;
        return q;
    }
    return NULL;
}

/* Free all storage used by queue */
void q_free(queue_t *q)
{
    if (q) {
        list_ele_t *it = q->head;
        while (it) {
            list_ele_t *tmp = it;
            it = it->next;
            free(tmp->value);
            free(tmp);
        }
        free(q);
    }
}

/*
 * Attempt to insert element at head of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_head(queue_t *q, char *s)
{
    if (q) {
        list_ele_t *newh = malloc(sizeof(list_ele_t));
        if (newh) {
            size_t size = strlen(s) + 1;
            newh->value = malloc(sizeof(char) * size);
            // newh->value = malloc(rlp.rlim_max);
            if (newh->value) {
                snprintf(newh->value, size, "%s", s);

                newh->next = q->head;
                q->head = newh;
                if (!q->tail)
                    q->tail = newh;
                q->size++;
                return true;
            }
            free(newh);
        }
    }
    return false;
}

/*
 * Attempt to insert element at tail of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_tail(queue_t *q, char *s)
{
    if (q) {
        list_ele_t *newt = malloc(sizeof(list_ele_t));
        if (newt) {
            newt->next = NULL;
            size_t size = strlen(s) + 1;
            newt->value = malloc(sizeof(char) * size);
            if (newt->value) {
                snprintf(newt->value, size, "%s", s);
                if (!q->tail) {
                    q->head = newt;
                    q->tail = newt;
                } else {
                    q->tail->next = newt;
                    q->tail = newt;
                }
                q->size++;
                return true;
            }
            free(newt);
        }
    }
    return false;
}

/*
 * Attempt to remove element from head of queue.
 * Return true if successful.
 * Return false if queue is NULL or empty.
 * If sp is non-NULL and an element is removed, copy the removed string to *sp
 * (up to a maximum of bufsize-1 characters, plus a null terminator.)
 * The space used by the list element and the string should be freed.
 */
bool q_remove_head(queue_t *q, char *sp, size_t bufsize)
{
    if (q && q->size > 0) {
        if (sp) {
            size_t size = strlen(q->head->value) + 1;
            snprintf(sp, size > bufsize ? bufsize : size, "%s", q->head->value);
        }

        list_ele_t *tmp = q->head;
        q->head = q->head->next;
        free(tmp->value);
        free(tmp);

        q->size--;
        return true;
    }
    return false;
}

/*
 * Return number of elements in queue.
 * Return 0 if q is NULL or empty
 */
int q_size(queue_t *q)
{
    return q ? q->size : 0;
}

/*
 * Reverse elements in queue
 * No effect if q is NULL or empty
 * This function should not allocate or free any list elements
 * (e.g., by calling q_insert_head, q_insert_tail, or q_remove_head).
 * It should rearrange the existing ones.
 */
void q_reverse(queue_t *q)
{
    if (q) {
        list_ele_t *h = q->head;
        list_ele_t *t = q->tail;

        list_ele_t *prev = NULL;
        list_ele_t *cur = q->head;

        while (cur) {
            list_ele_t *next = cur->next;
            cur->next = prev;
            prev = cur;
            cur = next;
        }
        q->head = t;
        q->tail = h;
    }
}

void merge(list_ele_t **result, list_ele_t *left, list_ele_t *right)
{
    while (left && right) {
        if (strcmp(left->value, right->value) > 0) {
            *result = right;
            right = right->next;
        } else {
            *result = left;
            left = left->next;
        }
        result = &((*result)->next);
    }

    *result = right ? right : left;
}

void mergesort(list_ele_t **head)
{
    if ((*head)->next) {
        list_ele_t *fast = *head;
        list_ele_t *slow = *head;
        list_ele_t *prev = NULL;

        while (fast && fast->next) {
            fast = fast->next->next;
            prev = slow;
            slow = slow->next;
        }
        if (prev)
            prev->next = NULL;

        mergesort(head);
        mergesort(&slow);

        list_ele_t *result = NULL;
        merge(&result, *head, slow);
        *head = result;
    }
}

/*
 * Sort elements of queue in ascending order
 * No effect if q is NULL or empty. In addition, if q has only one
 * element, do nothing.
 */
void q_sort(queue_t *q)
{
    if (q && q->size > 1) {
        mergesort(&q->head);
        list_ele_t *tmp = q->head;
        while (tmp->next) {
            tmp = tmp->next;
        }
        q->tail = tmp;
    }
}
