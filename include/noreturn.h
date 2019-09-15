#ifndef _NORETURN_H_
#define _NORETURN_H_

#ifdef __GNUC__
    #include <stdnoreturn.h>
#else
    #define noreturn
#endif

#endif
