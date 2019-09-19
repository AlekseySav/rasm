#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include <noreturn.h>
#include <flags.h>

// #define NO_EXTEND_WRITE

#if !defined(NO_EXTEND_WRITE) && !defined(__linux__)
    #define NO_EXTEND_WRITE
#endif

const char * progname = NULL;

void setprogname(const char * ptr)
{
    progname = ptr;
}

const char * getprogname()
{
    return progname;
}

noreturn void die(const char * fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);

    exit(1);
}

noreturn void panic(const char * err)
{
#ifndef NO_EXTEND_WRITE
    die("\033[1m%s: \033[31mfatal error:\033[0m %s\n", getprogname(), err);
#else
    die("%s: fatal error: %s\n", getprogname(), err);
#endif
}

static void echo(FILE * f, const char * fmt, va_list args)
{
    vfprintf(f, fmt, args);
    fprintf(f, "\n");
}

void print(int err, const char * str)
{
    if(!err && FLAG_CHECK(_Wall))
        return;
        
    fprintf((err || FLAG_CHECK(_Werror)) ? stderr : stdout, "%s", str);
}

void errorp(const char * file, int line, int col)
{
#ifndef NO_EXTENDED_WRITE
    fprintf(stderr, "\033[1m%s:%d:%d: %s: \033[0m", file, line, col, "\033[31merror");
#else
    fprintf(stderr, "%s:%d:%d: %s: ", file, line, col, "error");
#endif
}

void warnp(const char * file, int line, int col)
{
#ifndef NO_EXTENDED_WRITE
    fprintf(FLAG_CHECK(_Werror) ? stderr : stdout, "\033[1m%s:%d:%d: %s: \033[0m", file, line, col, "\033[35mwarning");
#else
    fprintf(FLAG_CHECK(_Werror) ? stderr : stdout, "%s:%d:%d: %s: ", file, line, col, "warning");
#endif
}

noreturn void errorf(const char * file, int line, int col, const char * msg, ...)
{
    va_list ap;
    va_start(ap, msg);
    errorp(file, line, col);
    echo(stderr, msg, ap);
    va_end(ap);
    exit(1);
}

void warnf(const char * file, int line, int col, const char * msg, ...)
{
    if(FLAG_CHECK(_Wall))
        return;

    va_list ap;
    va_start(ap, msg);
    warnp(file, line, col);
    echo(FLAG_CHECK(_Werror) ? stderr : stdout, msg, ap);
    va_end(ap);

    if(FLAG_CHECK(_Werror))
        exit(1);
}
