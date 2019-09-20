#ifndef _ERROR_H_
#define _ERROR_H_

void setprogname(const char * ptr);
const char * getprogname(void);

void die(const char * fmt, ...);
void panic(const char * err);

void rexit(int err);
void print(int err, const char * str);  // if(err) -> error else warning

void errorp(const char * file, int line, int col);  // write f:r:c error:
void warnp(const char * file, int line, int col);  // write f:r:c warning:

void errorf(const char * file, int line, int col, const char * msg, ...);
void warnf(const char * file, int line, int col, const char * msg, ...);

#endif
