#ifndef _BOOL_H_
#define _BOOL_H_

#define true 1
#define false 0

#ifdef __GNUC__
    #define bool _Bool
    #define __bool_true_false_are_defined 1
#else
    typedef int bool;
#endif

#endif
