#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#include "intern.h"

#define INTERNING_POOL_SIZE 1024

#define HASH_START_SIZE 16 /* must be power of 2 */

struct __xs_interning {
    int lock;
    int index;
    unsigned size;
    unsigned total;
    struct __xs_pool *pool;
};

struct __xs_pool {
    struct xs_node *node[INTERNING_POOL_SIZE];
};

static struct __xs_interning __intern_ctx;

#define XS_LOCK()                                               \
    ({                                                            \
        while (__sync_lock_test_and_set(&(__intern_ctx.lock), 1)) { \
        }                                                         \
    })
#define XS_UNLOCK() ({ __sync_lock_release(&(__intern_ctx.lock)); })

static inline uint32_t hash_blob(const char *buffer, size_t len)
{
    const uint8_t *ptr = (const uint8_t *) buffer;
    size_t h = len;
    size_t step = (len >> 5) + 1;
    for (size_t i = len; i >= step; i -= step)
        h = h ^ ((h << 5) + (h >> 2) + ptr[i - 1]);
    return h == 0 ? 1 : h % INTERNING_POOL_SIZE;
}

struct xs_node* get_interning(char *str)
{
    if (!__intern_ctx.pool)
        return NULL;

    XS_LOCK();
    uint32_t hash = hash_blob(str, strlen(str));
    struct xs_node *n = __intern_ctx.pool->node[hash];
    while (n) {
        if (strcmp(n->data + 4, str) == 0) {
            XS_UNLOCK();
            return n;
        }
        n = n->next;
    }
    XS_UNLOCK();
    return NULL;
}

void add_interning_address(char *data)
{
    XS_LOCK();
    int len = strlen(data);
    uint32_t hash = hash_blob(data, len);

    if (!__intern_ctx.pool) {
        __intern_ctx.pool = malloc(sizeof(struct __xs_pool));
    }

    struct xs_node **n = &__intern_ctx.pool->node[hash];
    while (*n) {
        if (strcmp((*n)->data + 4, data) == 0) {
            ++(*(int *) ((size_t) (*n)->data));
            XS_UNLOCK();
            return;
        }
        n = &((*n)->next);
    }
    *n = malloc(sizeof(struct xs_node));
    (*n)->data = data;
    XS_UNLOCK();
    return;
}

struct xs_node *add_interning(const char *str)
{
    XS_LOCK();
    int len = strlen(str);
    uint32_t hash = hash_blob(str, len);

    if (!__intern_ctx.pool) {
        __intern_ctx.pool = malloc(sizeof(struct __xs_pool));
    }

    struct xs_node **n = &__intern_ctx.pool->node[hash];
    while (*n) {
        if (strcmp((*n)->data + 4, str) == 0) {
            ++(*(int *) ((size_t) (*n)->data));
            XS_UNLOCK();
            return *n;
        }
        n = &((*n)->next);
    }
    *n = malloc(sizeof(struct xs_node));
    (*n)->data = malloc(sizeof(char) * len + 4);
    memcpy(((*n)->data + 4), str, len);
    XS_UNLOCK();
    return (*n);
}

void remove_interning(char *str)
{
    if (!__intern_ctx.pool)
        return;
    XS_LOCK();
    uint32_t hash = hash_blob(str, strlen(str));
    struct xs_node *n = __intern_ctx.pool->node[hash];
    while (n) {
        if (strcmp(n->data + 4, str) == 0) {
            --(*(int *) ((size_t) n->data));
            break;
        }
        n = n->next;
    }
    XS_UNLOCK();
}
