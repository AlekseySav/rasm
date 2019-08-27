#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string>
#include <vector>

#include <rasm/lex.h>
#include <rasm/errno.h>
#include <rasm/file.h>
#include <rasm/flags.h>

_Noreturn void panic(const char * err)
{
    #ifdef __linux__
        fprintf(stderr, "\033[1mrasm: \033[31mfatal error:\033[0m %s\n", err);
    #else
        fprintf(stderr, "rasm: fatal error: %s\n", err);
    #endif

    exit(1);
}

static void echo_err(int code, const char * err, const char * fmt, va_list args)
{
    FILE * f = code ? stderr : stdout;
    
    #ifdef __linux__
        fprintf(f, "\033[1m%s:%d:%d: ", filename(), line(), column());
    #else
        fprintf(f, "%s:%d:%d: ", filename(), line(), column());
    #endif

    fprintf(f, "%s", err);
    vfprintf(f, fmt, args);
    fprintf(f, "\n");
}

_Noreturn void errorf(const char * fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);

    #ifdef __linux__
        echo_err(1, "\033[31merror: \033[0m", fmt, ap);
    #else
        echo_err(1, "error: ", fmt, ap);
    #endif
    
    va_end(ap);
    exit(1);
}

void warnf(const char * fmt, ...)
{
    if(CHECK_FLAG(_Wall))
        return;
        
    va_list ap;
    va_start(ap, fmt);
    int code = CHECK_FLAG(_Werror) ? 1 : 0;

    #ifdef __linux__
        echo_err(code, "\033[35mwarning: \033[0m", fmt, ap);
    #else
        echo_err(code, "warning: ", fmt, ap);
    #endif

    va_end(ap);

    if(code)
        exit(code);
}

std::string to_str(token t)
{
    if(t.type == TNULL && t.str != "")
        return t.str;

    switch(t.type) {
        case TEOF:
            return "(eof)";
        case TLINE:
            return "(new-line)";
        case TSPACE:
            return "(space)";
        case TCOMMA:
            return ",";
        case TOP:
            return { t.op };
        case TSTR:
            return "\"" + t.str + "\"";

        case TCONST:
            return ".const";
        case TMACRO:
            return ".macro";
        case TIF:
            return ".if";
        case TELSE:
            return ".else";
        case TEND:
            return ".end";
        case TRELEASE:
            return ".release";
        case TINCLUDE:
            return ".include";
        case TWARNING:
            return ".warning";
        case TERROR:
            return ".error";
        default:
            return "(null)";
    }
}
