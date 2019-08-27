#ifndef _ERRNO_H_
#define _ERRNO_H_

#ifndef _Noreturn
    #ifdef __GNUC__
        #define _Noreturn __attribute__((__noreturn__))
    #else
        #define _Noreturn
    #endif
#endif

_Noreturn void panic(const char * err);

_Noreturn void errorf(const char * fmt, ...);
void warnf(const char * fmt, ...);

std::string to_str(token t);

#endif
