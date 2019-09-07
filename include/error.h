#ifndef _ERROR_H_
#define _ERROR_H_

void setprogname(const char * ptr);
const char * getprogname(void);

void die(const char * fmt, ...);
void panic(const char * err);

void errorf(const char * file, int line, int col, const char * msg, ...);
void warnf(const char * file, int line, int col, const char * msg, ...);

#endif
