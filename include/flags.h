#ifndef _FLAGS_H_
#define _FLAGS_H_

extern int flags;

#define _E          1
#define _nostdinc   2
#define _Wall       4
#define _Werror     8

#define FLAG_CHECK(f)   (flags & f)
#define FLAG_ADD(f)     (flags |= f)

#endif
