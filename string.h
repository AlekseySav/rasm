#ifndef _STRING_H_
#define _STRING_H_

#include <memory.h>

struct string_t {
    char buf[1024];
    size_t len;

    string_t()
    {
        *buf = '\0';
        len = 0;
    }

    string_t(const char * ptr)
    {
        len = 0;
        while(*ptr != 0) {
            buf[len] = *ptr++;
            len++;
        }
    }

    string_t(char * buf, size_t len)
    {
        memcpy(this->buf, buf, len);
        this->len = len;
    }

    const char * str()
    {
        buf[len] = '\0';
        return buf;
    }

    string_t operator+ (string_t s)
    {
        string_t s1 = this->str();
        memcpy(s1.buf + s1.len, s.buf, s.len);
        s1.len += s.len;
        return s1;
    }

    string_t operator+= (string_t s)
    {
        string_t s1 = this->operator+(s);
        memcpy(this->buf, s1.buf, s1.len);
        this->len = s1.len;
        return s1;
    }

    vector_t<string_t> split(char symbol = ' ')
    {
        vector_t<string_t> v;
        char * ptr = buf;
        size_t len = 0;
        
        for(size_t i = 0; i < this->len; i++) {
            if(buf[i] != symbol) len++;
            else {
                v.append(string_t(ptr, len));
                i++;
                ptr = buf + i;
                len = 1;
            }
        }
        v.append(string_t(ptr, len));

        return v;
    }

    string_t remove_trash()
    {
        string_t s = string_t(this->buf, this->len);
        char * ptr = s.buf;

        while(*ptr == ' ' || *ptr == '\t') {
            ptr++;
            s.len--;
        }
        memcpy(s.buf, ptr, s.len);
        
        vector_t<string_t> v = s.split('|');
        s = v[0];
        
        ptr = s.buf + s.len - 1;
        while(*ptr == ' ' || *ptr == '\t') {
            ptr--;
            s.len--;
        }

        return s;
    }

    string_t remove_start(size_t c)
    {
        string_t s = string_t(this->buf, this->len);
        s.len -= c;
        memcpy(s.buf, s.buf + c, s.len);
        return s;
    }

};

#endif
