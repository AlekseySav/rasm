#ifndef _VECTOR_H_
#define _VECTOR_H_

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

void die(const char * ptr)
{
    fprintf(stderr, "E: %s\n", ptr);
    exit(1);
}

template<typename Ty>
struct vector_t {
    Ty src[64];
    size_t len;

    vector_t()
    {
        len = 0;
    }

    vector_t(Ty * ptr, size_t len)
    {
        this->len = len;
        for(size_t i = 0; i < len; i++) {
            src[i] = ptr[i];
        }
    }

    void append(Ty t)
    {
        if(len >= 1024)
            die("Limit of vector size ended");
        src[len] = t;
        len++;
    }

    Ty operator[] (size_t i)
    {
        if(i >= len)
            die("Index of vector bigger than it's size");
        return src[i];
    }

    void set(size_t i, Ty v)
    {
        if(i >= len)
            die("Index of vector bigger than it's size");

        src[i] = v;
    }

};

#endif
