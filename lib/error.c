#include <stdnoreturn.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include <flags.h>

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

static void echo(FILE * f, const char * file, int line, int col, const char * error, const char * fmt, va_list args)
{
#ifndef NO_EXTENDED_WRITE
    fprintf(f, "\033[1m%s:%d:%d: %s: \033[0m", file, line, col, error);
#else
    fprintf(f, "%s:%d:%d: %s: ", file, line, col, error);
#endif
    vfprintf(f, fmt, args);
    fprintf(f, "\n");
}

noreturn void errorf(const char * file, int line, int col, const char * msg, ...)
{
    va_list ap;
    va_start(ap, msg);
#ifndef NO_EXTENDED_WRITE
    echo(stderr, file, line, col, "\033[31merror", msg, ap);
#else
    echo(stderr, file, line, col, "error", msg, ap);
#endif
    va_end(ap);
    exit(1);
}

void warnf(const char * file, int line, int col, const char * msg, ...)
{
    if(FLAG_CHECK(_Wall))
        return;

    va_list ap;
    va_start(ap, msg);
#ifndef NO_EXTENDED_WRITE
    echo(FLAG_CHECK(_Werror) ? stderr : stdout, file, line, col, "\033[35mwarning", msg, ap);
#else
    echo(FLAG_CHECK(_Werror) ? stderr : stdout, file, line, col, "warning", msg, ap);
#endif
    va_end(ap);

    if(FLAG_CHECK(_Werror))
        exit(1);
}
