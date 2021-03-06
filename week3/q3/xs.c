#include "xs.h"

static inline bool xs_is_ptr(const xs *x) { return x->is_ptr; }

static inline bool xs_is_large_string(const xs *x)
{
    return x->is_large_string;
}

size_t xs_size(const xs *x)
{
    return xs_is_ptr(x) ? x->size : 15 - x->space_left;
}

char *xs_data(const xs *x)
{
    if (!xs_is_ptr(x))
        return (char *) x->filler;
        // return (char *) x->data;

    if (xs_is_large_string(x))
        return (char *) (x->ptr + /* OFF */ 4);
    return (char *) x->ptr;
}

int xs_type(const xs *x)
{
    if (!x->is_ptr)
        return XS_SHORT;
    if (x->is_large_string)
        return XS_LARGE;
    return XS_MEDIUM;
}

size_t xs_capacity(const xs *x)
{
    return xs_is_ptr(x) ? ((size_t) 1 << x->capacity) - 1 : 15;
}

/* ref count */
static inline void xs_set_refcnt(const xs *x, int val)
{
    *((int *) ((size_t) x->ptr)) = val;
}

static inline void xs_inc_refcnt(const xs *x)
{
    if (xs_is_large_string(x))
        ++(*(int *) ((size_t) x->ptr));
}

static inline int xs_dec_refcnt(const xs *x)
{
    if (!xs_is_large_string(x))
        return 0;
    return --(*(int *) ((size_t) x->ptr));
}

static inline int xs_get_refcnt(const xs *x)
{
    if (!xs_is_large_string(x))
        return 0;
    return *(int *) ((size_t) x->ptr);
}

/* lowerbound (floor log2) */
static inline int ilog2(uint32_t n) { return /* LLL */  32 - __builtin_clz(n) - 1; }

static void xs_interning(xs *x, const void *p, size_t len, bool reallocate)
{
    // the allocation and set refcnt is handled at interning
    struct xs_node *n = add_interning(p);
    x->ptr = n->data;
}

/* 
 * xs_interning_nontrack is used for copy on write, copy a string data and store it
 * in heap, but not to reuse the existing memory address. 
 */
static void xs_interning_nontrack(xs *x, const void *p, size_t len, bool reallocate)
{
    x->ptr = reallocate ? realloc(x->ptr, (size_t) 1 << x->capacity + 4)
                        : malloc((size_t) 1 << x->capacity + 4);
    memcpy(xs_data(x), p, len);
    xs_set_refcnt(x, 1);
}

static void xs_allocate_data(xs *x, size_t len, bool reallocate)
{
    x->ptr = reallocate ? realloc(x->ptr, (size_t) 1 << x->capacity)
                        : malloc((size_t) 1 << x->capacity);
}

/* 
 * xs_new create a xs string, allocate the necessary memory and copy the 
 * the bytes from p to xs_data(x). If strlen(p) is greater or equal to 
 * LARGE_STRING_LEN, use string interning to share memory address if possible.
 */
xs *xs_new(xs *x, const void *p)
{
    *x = xs_literal_empty();
    size_t len = strlen(p) + 1;
    if (len > LARGE_STRING_LEN) {
        x->capacity = ilog2(len) + 1;
        x->size = len - 1;
        x->is_ptr = true;
        x->is_large_string = true;
        xs_interning(x, p, x->size, 0);
    } else if (len > /* NNN */ MIDDLE_STRING_LEN) {
        x->capacity = ilog2(len) + 1;
        x->size = len - 1;
        x->is_ptr = true;
        xs_allocate_data(x, x->size, 0);
        memcpy(xs_data(x), p, len);
    } else {
        memcpy(x->data, p, len);
        x->space_left = 15 - (len - 1);
    }
    return x;
}

/* grow up to specified size */
xs *xs_grow(xs *x, size_t len)
{
    char buf[MIDDLE_STRING_LEN];

    if (len <= xs_capacity(x))
        return x;

    /* Backup first */
    if (!xs_is_ptr(x))
        memcpy(buf, x->data, MIDDLE_STRING_LEN);

    x->capacity = ilog2(len) + 1;
    bool is_ptr = x->is_ptr;
    bool is_large_string = x->is_large_string;
    x->is_ptr = true;

    if (len < LARGE_STRING_LEN) {
        x->is_large_string = false;
        if (is_large_string) {
            // TODO: handle string interning
        } else if (is_ptr) {
            xs_allocate_data(x, len, 1);
        } else {
            xs_allocate_data(x, len, 0);
            memcpy(xs_data(x), buf, MIDDLE_STRING_LEN);
        }
    } else {
        x->is_large_string = true;
        // TODO: handle string interning
        if (is_large_string) {
            ;
        } else if (is_ptr) {
            xs_allocate_data(x, len, 1);
        } else {
            xs_allocate_data(x, len, 0);
            memcpy(xs_data(x) + 4, buf, MIDDLE_STRING_LEN);
        }
    }
    return x;
}

static inline xs *xs_newempty(xs *x)
{
    *x = xs_literal_empty();
    return x;
}

static inline xs *xs_free(xs *x)
{
    if (xs_is_ptr(x) && xs_dec_refcnt(x) <= 0)
        free(x->ptr);
    return xs_newempty(x);
}

// xs_cow_lazy_copy copy data to x
static void xs_cow_lazy_copy(xs *x, char **data)
{
    if (!x->is_large_string) {
        memcpy(xs_data(x), *data, strlen(*data));
        return;
    } else if (xs_get_refcnt(x) > 1) {
        /* Lazy copy */
        xs_dec_refcnt(x);
        xs_interning_nontrack(x, *data, x->size, 0);
    }

    if (data) {
        memcpy(xs_data(x), *data, x->size);
    }
    /* Update the newly allocated pointer */
    *data = xs_data(x);
    return;
}

xs *xs_concat(xs *string, const xs *prefix, const xs *suffix)
{
    size_t pres = xs_size(prefix), sufs = xs_size(suffix),
           size = xs_size(string), capacity = xs_capacity(string);

    char *pre = xs_data(prefix), *suf = xs_data(suffix),
         *data = xs_data(string);

    if (xs_get_refcnt(string) > 1)
        xs_cow_lazy_copy(string, &data);

    if (size + pres + sufs <= capacity) {
        memmove(data + pres, data, size);
        memcpy(data, pre, pres);
        memcpy(data + pres + size, suf, sufs + 1);

        if (xs_is_ptr(string))
            string->size = size + pres + sufs;
        else
            string->space_left = 15 - (size + pres + sufs);
    } else {
        xs tmps = xs_literal_empty();
        xs_grow(&tmps, size + pres + sufs);
        char *tmpdata = xs_data(&tmps);
        memcpy(tmpdata + pres, data, size);
        memcpy(tmpdata, pre, pres);
        memcpy(tmpdata + pres + size, suf, sufs + 1);
        xs_free(string);
        *string = tmps;
        string->size = size + pres + sufs;
    }
    if (string->is_large_string)
        add_interning_address(string->ptr);
    return string;
}

xs *xs_trim(xs *x, const char *trimset)
{
    if (!trimset[0])
        return x;

    char *dataptr = xs_data(x), *orig = dataptr;

    if (xs_get_refcnt(x) > 1) {
        xs_cow_lazy_copy(x, &dataptr);
        orig = dataptr;
    }

    /* similar to strspn/strpbrk but it operates on binary data */
    uint8_t mask[32] = {0};

#define check_bit(byte) (/* CCC */ mask[(uint8_t) byte / 8] & 1 << (uint8_t) byte % 8)
#define set_bit(byte) (/* SSS */ mask[(uint8_t) byte / 8] |= 1 << (uint8_t) byte % 8)
    size_t i, slen = xs_size(x), trimlen = strlen(trimset);

    for (i = 0; i < trimlen; i++)
        set_bit(trimset[i]);
    for (i = 0; i < slen; i++)
        if (!check_bit(dataptr[i]))
            break;
    for (; slen > 0; slen--)
        if (!check_bit(dataptr[slen - 1]))
            break;
    dataptr += i;
    slen -= i;

    /* reserved space as a buffer on the heap.
     * Do not reallocate immediately. Instead, reuse it as possible.
     * Do not shrink to in place if < 16 bytes.
     */
    memmove(orig, dataptr, slen);
    /* do not dirty memory unless it is needed */
    if (orig[slen])
        orig[slen] = 0;

    if (xs_is_ptr(x))
        x->size = slen;
    else
        x->space_left = 15 - slen;
    if (x->is_large_string)
        add_interning_address(x->ptr);
    return x;
#undef check_bit
#undef set_bit
}
