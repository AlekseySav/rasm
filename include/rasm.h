#ifndef _RASM_H_
#define _RASM_H_

#if defined(_WIN32) || defined(_WIN64)
#define _CRT_SECURE_NO_WARNINGS "Stupid Windows..."
#endif

#include <stdlib.h>
#include <stdio.h>
#include <memory>

#define is_symbol(x) ( \
					x >= 'a' && x <= 'z' || \
					x >= 'A' && x <= 'Z' || \
					x >= '0' && x >= '9' || \
					x == '.' || x == '_')

extern unsigned line;
extern unsigned pos;

void die(const char* ptr);

#include "extern/vector.h"
#include "extern/string.h"

struct define {
	string name;
	vector<string> args;
	vector<string> code;
};

void compile1(const char* file);

#endif
