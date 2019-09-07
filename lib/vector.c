#include <stddef.h>
#include <stdlib.h>
#include <assert.h>
#include <memory.h>

#include <vector.h>
#include <error.h>

static inline void vec_check(vector * v)
{
    assert(v);
    assert(v->buf);
}

static vector * vec_alloc(size_t alloc_size)
{
    assert(alloc_size);

    vector * v = malloc(sizeof(vector));
    assert(v);

    v->buf = malloc(sizeof(void *) * alloc_size);
    assert(v->buf);

    v->len = 0;
    v->bufsz = alloc_size;

    return v;
}

static void vec_realloc(vector * v)
{
    v->buf = realloc(v->buf, sizeof(void *) * v->bufsz);
    assert(v->buf);
}

vector * vec_create(void)
{
    return vec_alloc(1);
}

void vec_free(vector * v)
{
    if(v == NULL)
        return;
    
    assert(v->buf);
    free(v->buf);
    free(v);
}

void vec_push(vector * v, const void * value)
{
    vec_check(v);
    
    v->len++;
    if(v->len > v->bufsz) {
        v->bufsz *= 2;
        vec_realloc(v);
    }

    v->buf[vec_len(v) - 1] = value;
}

const void * vec_pop(vector * v)
{
    vec_check(v);
    
    if(vec_len(v) == 0)
        panic("vector index out of range");

    const void * value = v->buf[--v->len];

    if(v->len * 2 <= v->bufsz && v->bufsz != 1) {
        v->bufsz /= 2;
        vec_realloc(v);
    }

    return value;
}

void vec_set(vector * v, size_t index, const void * value)
{
    vec_check(v);
    
    if(v->len <= index)
        panic("vector index out of range");

    v->buf[index] = value;
}

const void * vec_get(vector * v, size_t index)
{
    vec_check(v);
    
    if(v->len <= index)
        panic("vector index out of range");

    return v->buf[index];
}

vector * vec_cpy(vector * src)
{
    vector * v = vec_alloc(src->bufsz);
    memcpy(v, src, vec_len(v));
    return v;
}
