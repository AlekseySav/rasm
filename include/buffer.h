#ifndef _BUFFER_H_
#define _BUFFER_H_

typedef struct {
    char * buf;
    size_t len;
    size_t bufsz;
} buffer;

#define buf_len(b) ((b)->len)
#define buf_tail(b) (buf_get(b, buf_len(b) - 1))
#define buf_nil(b) (buf_create(""))
#define buf_release(b) (buf_free(b), ((b) = NULL))  // always returns NULL

buffer * buf_create(const char * src);
void buf_free(buffer * b);

void buf_push(buffer * b, char value);
char buf_pop(buffer * b);

void buf_set(buffer * b, size_t index, char value);
char buf_get(buffer * b, size_t index);

const char * buf_cstr(buffer * b);

void buf_append(buffer * dest, const char * src);

#endif
