#ifndef _FLAGS_H_
#define _FLAGS_H_

extern int flags;

#define _nostdinc   1
#define _Wall       2
#define _Werror     4

#define FLAG_CHECK(f)   (flags & f)
#define FLAG_ADD(f)     (flags |= f)

#endif
