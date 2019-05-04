#ifndef _RASM_H_
#define _RASM_H_

#define FLAG_NOCLEAR 	1
#define FLAG_LOG		2
#define FLAG_VERSION	4

extern int flags;

#include <stdlib.h>
#include <stdio.h>
#include <memory>
#include <memory.h>
#include <fstream>
#include <iostream>

#define is_symbol(x) ( \
					x >= 'a' && x <= 'z' || \
					x >= 'A' && x <= 'Z' || \
					x >= '0' && x <= '9' || \
					x == '.' || x == '_')

extern unsigned part;
extern unsigned line;
extern unsigned pos;

void die(const char* ptr);

#include "extern/vector.h"
#include "extern/string.h"

void compile1(const char* file, const char* of);
void compile2(const char* file, const char* of);
void compile3(const char* file, const char* of);

#endif
