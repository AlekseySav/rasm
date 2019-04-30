#include <iostream>
#include <vector>

typedef unsigned uint;
typedef std::vector<int> arg;

typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned int dword;

typedef struct {
	char * ptr;
	uint len;
} strpart;

enum atypes {
	A_INT = 1,
	A_STR = 2
};

typedef struct {
	atypes type;
	void * a;
} args_t;

enum ctypes {
	C_DECLARE = 1
};

enum stypes {
	S_BYTE = 1,
	S_WORD = 2,
	S_DWORD = 4
};

typedef struct {
	ctypes type;
	stypes size;
	args_t args;
} command_t;

bool cstr(strpart s, const char* c)
{
	while (*c != 0)
		if (*s.ptr++ != *c++)
			return false;
	return true;
}

#define NEG 1
#define HEX 2
#define OCT 4
#define BIN 8

int toint(strpart s)
{
	int flag = 0;

	if (*s.ptr == '-') {
		flag |= NEG;
		s.ptr++;
		s.len--;
	}

	if (*s.ptr == '0') {
		flag |= OCT;
		s.ptr++;
		s.len--;
		if (*s.ptr == 'x' || *s.ptr == 'X') {
			flag |= HEX;
			s.ptr++;
			s.len--;
		}
	}

	if (*(s.ptr + s.len - 1) == 'b' && !(flag & HEX)) {
		flag |= BIN;
		s.len--;
	}

	int i = 0;

	while (s.len--) {
		if (flag & BIN) i = i * 2 + *s.ptr - '0';
		else if (flag & HEX) {
			i = i * 16 + *s.ptr;
			if (*s.ptr >= '0' && *s.ptr <= '9')
				i -= '0' ;
			else if (*s.ptr >= 'a' && *s.ptr <= 'z')
				i -= 'a' - 10;
			else
				i -= 'A' - 10;
		}
		else if (flag & OCT)
			i = i * 8 + *s.ptr - '0';
		else // DEC
			i = i * 10 + *s.ptr - '0';
		s.ptr++;
	}

	if (flag & NEG) i = -i;
	return i;
}

int compile_command(command_t c, byte * buf)
{
	uint i = 0;
	strpart s;
	arg a;
	byte * b = buf;

	switch (c.type)
	{
	case C_DECLARE:
		switch (c.size)
		{
		case S_BYTE:
			if (c.args.type == A_STR) {
				s = *(strpart*)c.args.a;
				while (s.len--) {
					*b++ = *s.ptr++;
					i++;
				}
				break;
			}
			a = *(arg*)c.args.a;
			for (i = 0; i < a.size(); i++) {
				*(byte*)b++ = (byte)a[i];
			}
			break;
		case S_WORD:
			a = *(arg*)c.args.a;
			for (i = 0; i < a.size(); i++) {
				*(word*)b = (word)a[i];
				b += 2;
			}
			break;
		case S_DWORD:
			a = *(arg*)c.args.a;
			for (i = 0; i < a.size(); i++) {
				*(dword*)b = (dword)a[i];
				b += 4;
			}
			break;
		}
		break;
	}
	return b - buf;
}

#define skip(x) while(*x == ' ' || *x == '\t') { x++; }

int compile_str(char * cmd, byte * buf)
{

	skip(cmd);
	
	strpart s;
	command_t c;
	arg a;
	s.len = 0;
	s.ptr = cmd;

	if (cstr(s, ".byte")) {
		c.type = C_DECLARE;
		c.size = S_BYTE;
		c.args.type = A_INT;
	}
	else if (cstr(s, ".word")) {
		c.type = C_DECLARE;
		c.size = S_WORD;
		c.args.type = A_INT;
	}
	else if (cstr(s, ".dword")) {
		c.type = C_DECLARE;
		c.size = S_WORD;
		c.args.type = A_INT;
	}
	else if (cstr(s, ".ascii")) {
		c.type = C_DECLARE;
		c.size = S_BYTE;
		c.args.type = A_STR;
	}
	else return 0;

	while (*cmd != ' ') cmd++;
	skip(cmd);

	if (c.args.type == A_STR) {
		while (*cmd != '"') cmd++;
		cmd++;
		s.ptr = cmd;
		while (*cmd != '"') {
			cmd++; 
			s.len++;
		}
		c.args.a = &s;
	}
	else {
		while (*cmd != '\n' && *cmd != '|') {
			s.ptr = cmd;
			s.len = 0;
			while (*cmd != ' ' && *cmd != '\t' && *cmd != ',' && *cmd != '\n' && *cmd != '|') {
				cmd++;
				s.len++;
			}
			skip(cmd);
			if(*cmd == ',') cmd++;
			skip(cmd);
			a.push_back(toint(s));
		}
		c.args.a = &a;
	}

	return compile_command(c, buf);
}

#define BUF_SIZ 10

void compiler(char* ptr)
{
	byte buf[BUF_SIZ];
	byte * b = buf;
	for (int i = 0; i < BUF_SIZ; i++) buf[i] = 0;

	while (*ptr != 0) {
		b += compile_str(ptr, b);
		while (*ptr != '\n') ptr++;
		ptr++;
	}

	for (int i = 0; i < BUF_SIZ; i++)
		printf("%#x ", (unsigned)buf[i]);
	printf("\n");
	for (int i = 0; i < BUF_SIZ; i++)
		printf("%c", buf[i]);
}

int main(void)
{
	char* ptr = (char *)".word 0xaa55\n| comment\n\t   .ascii \"string\" | end";
	compiler(ptr);
}
