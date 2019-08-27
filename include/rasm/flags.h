#ifndef _FLAGS_H_
#define _FLAGS_H_

#define _Wall       1
#define _Werror     2

extern int flags;

#define CHECK_FLAG(f) (flags & f)

#endif
