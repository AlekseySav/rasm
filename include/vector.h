#ifndef _VECTOR_H_
#define _VECTOR_H_

typedef struct {
    const void ** buf;
    size_t len;
    size_t bufsz;
} vector;

#define vec_len(v) ((v)->len)
#define vec_tail(v) (vec_get(v, vec_len(v) - 1))

#define vec_nil() (vec_create())
#define vec_release(v) (vec_free(v), ((v) = NULL))  // always returns NULL

vector * vec_create(void);
void vec_free(vector * v);

void vec_push(vector * v, const void * value);
const void * vec_pop(vector * v); 

void vec_set(vector * v, size_t index, const void * value);
const void * vec_get(vector * v, size_t index);

vector * vec_cpy(vector * src);

#endif
