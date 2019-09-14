#include <stddef.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include <buffer.h>
#include <error.h>

#define MINSIZ  8

static size_t buf_round(size_t n)
{
    if(n == 0)
        return 0;

    int res = 1;
    while(res <= n)
        res *= 2;
    return res;
}

static void buf_realloc(buffer * b, size_t nl)
{
    assert(b);

    //b->len = nl;
    b->bufsz = buf_round(nl);
    if(b->bufsz < MINSIZ)
        b->bufsz = MINSIZ;

    if(b->buf)
        b->buf = realloc(b->buf, b->bufsz);
    else b->buf = malloc(b->bufsz);
    assert(b);
}

buffer * buf_create(const char * src)
{
    buffer * b = calloc(1, sizeof(buffer));
    assert(b);

    b->len = strlen(src);
    buf_realloc(b, b->len);
    if(b->len) {
        strcpy(b->buf, src);
    }
    return b;
}

void buf_free(buffer * b)
{
    if(b == NULL)
        return;
    
    if(b->buf) free(b->buf);
    free(b);
}

void buf_push(buffer * b, char value)
{
    assert(b);
    b->buf[buf_len(b)++] = value;
    buf_realloc(b, buf_len(b));
}

char buf_pop(buffer * b)
{
    assert(b);
    if(buf_len(b) == 0)
        panic("buffer index out of range");

    char value = b->buf[--buf_len(b)];
    buf_realloc(b, buf_len(b));
    return value;
}

void buf_set(buffer * b, size_t index, char value)
{
    assert(b);
    if(buf_len(b) <= index)
        panic("buffer index out of range");
    b->buf[index] = value;
}

char buf_get(buffer * b, size_t index)
{
    assert(b);
    if(buf_len(b) <= index)
        panic("buffer index out of range");
    return b->buf[index];
}

const char * buf_cstr(buffer * b)
{
    b->buf[buf_len(b)] = 0;
    return (const char *)b->buf;
}

void buf_append(buffer * dest, const char * src)
{
    assert(dest);
    assert(src);
    size_t len = strlen(src);

    buf_realloc(dest, buf_len(dest) + len);
    memcpy(dest->buf + buf_len(dest), src, len);
    dest->len += len;
}
