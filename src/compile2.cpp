#include "rasm.h"

#define BUF_SIZE 1024

static char buf[BUF_SIZE];
static char* bp = buf;
static unsigned prev = 0;

struct command {
	string name;
	vector<string> args;
};

struct pointer {
	string name;
	unsigned value;
};

static vector<pointer> pointers = vector<pointer>();
static bool part1 = true;

#define NEG 1
#define HEX 2
#define OCT 4
#define BIN 8

static int toint(string s)
{
	for (size_t i = 0; i < pointers.len; i++)
		if (s == pointers[i].name)
			return pointers[i].value;

	char* ptr = s.buf;
	size_t len = s.len;

	int flag = 0;

	if (*ptr == '-') {
		flag |= NEG;
		ptr++;
		len--;
	}

	if (*ptr == '0') {
		flag |= OCT;
		ptr++;
		len--;
		if (*ptr == 'x' || *ptr == 'X') {
			flag |= HEX;
			ptr++;
			len--;
		}
	}

	if (*(ptr + len - 1) == 'b' && !(flag & HEX)) {
		flag |= BIN;
		len--;
	}

	int i = 0;

	while (len--) {
		if (flag & BIN) i = i * 2 + *ptr - '0';
		else if (flag & HEX) {
			i = i * 16 + *ptr;
			if (*ptr >= '0' && *ptr <= '9')
				i -= '0';
			else if (*ptr >= 'a' && *ptr <= 'z')
				i -= 'a' - 10;
			else
				i -= 'A' - 10;
		}
		else if (flag & OCT)
			i = i * 8 + *ptr - '0';
		else // DEC
			i = i * 10 + *ptr - '0';
		ptr++;
	}

	if (flag & NEG) i = -i;
	return i;
}

static void bpint(string s)
{
	char* ptr = s.buf;
	size_t len = 0;
	int res = 0;
	char oper = '+';
	while (ptr != s.buf + s.len ) {
		while (is_symbol(*ptr)) {
			ptr++;
			len++;
			if (ptr == s.buf + s.len)
				break;
		}
		int t = toint(string(ptr - len, len));
		switch (oper) {
		case '+':
			res += t;
			break;
		case '-':
			res -= t;
			break;
		case '*':
			res *= t;
			break;
		case '/':
			res /= t;
			break;
		case '%':
			res %= t;
			break;
		default:
			die("Unrecognized operand");
		}

		if (ptr == s.buf + s.len)
			break;
		while (*ptr == ' ' || *ptr == '\t')
			ptr++;
		
		oper = *ptr++;

		if (ptr == s.buf + s.len)
			break;
		while (*ptr == ' ' || *ptr == '\t')
			ptr++;

		len = 0;
	}

	bp += sprintf(bp, " %u", res);
}

static void compile_command(command c)
{
	size_t i;
	c.name.remove_trash();

	if (c.name[c.name.len - 1] == ':') {
		pointer p;
		p.name = c.name;
		p.name.len--;
		p.value = prev;
		pointers.add(p);
	}
	else if (strcmp(c.name.str(), ".byte") == 0) {
		if (!part1)
			* bp++ = 'b';
		for (i = 0; i < c.args.len; i++) {
			if (part1)
				prev++;
			else bpint(c.args[i]);
		}
		if (!part1)
			* bp++ = '\n';
	}
	else if (strcmp(c.name.str(), ".word") == 0) {
		if (!part1)
			* bp++ = 'w';
		for (i = 0; i < c.args.len; i++) {
			if (part1)
				prev += 2;
			else bpint(c.args[i]);
		}
		if (!part1)
			* bp++ = '\n';
	}
	else die("Unrecognized command");
}

static void compile_line(string s, unsigned ln)
{
	line = ln;
	
	if(flags & FLAG_LOG)
		printf("%s\n", s.str());

	string name;

	pos = 0;
	name = s.split()[0];

	pos += name.len;
	s = s.remove_start(name.len);
	s = s.remove_trash();

	vector<string> v = s.split(',');
	for (size_t i = 0; i < v.len; i++)
		v[i] = v[i].remove_trash();

	command c;
	c.name = name;
	c.args = v;
	compile_command(c);
}

void compile2(const char* file, const char* of)
{
	part = 2;
	string s = string();
	char ibuf = '\0';
	unsigned line = 1;

	FILE* f = fopen(file, "rt");
	while (fread(&ibuf, 1, 1, f) != 0) {
		if (ibuf != '\n')
			s.append(ibuf);
		else  {
			compile_line(s, line);
			line++;
			s = "";
		}
	}
	fclose(f);
	
	if(flags & FLAG_LOG)
		printf("\n+++\n\n");

	s = string();
	ibuf = '\0';
	line = 1;
	part1 = false;
	part = 3;
	bp = buf;

	f = fopen(file, "rt");
	FILE* f1 = fopen(of, "wt+");
	while (fread(&ibuf, 1, 1, f) != 0) {
		if (ibuf != '\n')
			s.append(ibuf);
		else {
			for (size_t i = 0; i < BUF_SIZE; i++)
				buf[i] = 0;
			compile_line(s, line);
			if (bp - buf != 0)
				fwrite(buf, 1, bp - buf, f1);
			bp = buf;
			line++;
			s = "";
		}
	}
	fclose(f);
	fclose(f1);
}
