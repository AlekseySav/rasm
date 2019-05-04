#include "rasm.h"

#define BUF_SIZE 1024
typedef unsigned char byte;
typedef unsigned short word;

static byte buf[BUF_SIZE];
static byte* bp = buf;

static unsigned toint(string s)
{
	unsigned res = 0;
	unsigned i = 0;

	while (i < s.len) {
		res = res * 10 + s[i] - '0';
		i++;
	}
	return res;
}

static void compile_line(string s, unsigned ln)
{
	line = ln;
	
	if(flags & FLAG_LOG)
		printf("%s\n", s.str());

	vector<string> v = s.split();
	if (v[0].len != 1)
		die("Unrecognized command");
	char name = v[0][0];

	switch (name) {
	case 'b':
		for (size_t i = 1; i < v.len; i++) {
			*(byte*)bp = (byte)toint(v[i]);
			bp++;
		}
		break;
	case 'w':
		for (size_t i = 1; i < v.len; i++) {
			*(word*)bp = (word)toint(v[i]);
			bp += 2;
		}
		break;
	}
}

void compile3(const char* file, const char* of)
{
	part = 4;
	string s = string();
	char ibuf = '\0';
	unsigned line = 1;

	FILE* f = fopen(file, "rt");
	FILE* f1 = fopen(of, "wt+");
	while (fread(&ibuf, 1, 1, f) != 0) {
		if (ibuf != '\n')
			s.append(ibuf);
		else {
			for (size_t i = 0; i < BUF_SIZE; i++)
				buf[i] = 0;
			compile_line(s, line);
			if ((byte*)bp - buf != 0)
				fwrite(buf, 1, (byte*)bp - buf, f1);
			bp = buf;
			line++;
			s = "";
		}
	}
	fclose(f);
	fclose(f1);
}
