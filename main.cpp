#include <stdio.h>
#include <vector>

typedef struct {
	char* ptr;
	int len;
} word;

word currcmd;
std::vector<word> args;
char obuf[1024];
int pos = 0;

#define skipspaces(buf) if(*buf == ' ' || *buf == '\t') { while(*++buf == ' ' || *buf == '\t'); }

void die(const char* ptr) {
	fprintf(stderr, "%s\n", ptr);
	exit(1);
}

int convert16(word w)
{
	int res = 0;
	for (int i = 0; i < w.len; i++) {
		if ('0' <= w.ptr[i] && w.ptr[i] <= '9')
			res = res * 16 + w.ptr[i] - '0';
		else if ('a' <= w.ptr[i] && w.ptr[i] <= 'f')
			res = res * 16 + w.ptr[i] - 'a' + 10;
		else if ('A' <= w.ptr[i] && w.ptr[i] <= 'F')
			res = res * 16 + w.ptr[i] - 'A' + 10;
		else die("Not correct number");
	}
	return res;
}

int convert10(word w)
{
	int res = 0;
	for (int i = 0; i < w.len; i++) {
		if ('0' <= w.ptr[i] && w.ptr[i] <= '9')
			res = res * 10 + w.ptr[i] - '0';
		else die("Not correct number");
	}
	return res;
}

bool cmpstr(char * s1, const char * s2, int len) 
{
	while (len--)
		if (*s1++ != *s2++)
			return false;
	return true;
}

void invoke(char* buf)
{
	skipspaces(buf);
	if (*buf == '\n') return;
	currcmd.ptr = buf;
	currcmd.len = 0;
	while (*buf++ != ' ') currcmd.len++;

	word arg;
	skipspaces(buf);
	while (*buf != '\n') {
		arg.ptr = buf;
		arg.len = 0;
		while (*buf != '\n' && *buf != ',' && *buf != ' ' && *buf != '\t' && *buf != '|') {
			arg.len++;
			buf++;
		}
		if (*buf == ',') buf++;
		args.push_back(arg);
		skipspaces(buf);
	}
}

void convert_args(void)
{
	for (int i = 0; i < (int)args.size(); i++) {
		if (args[i].ptr[0] == '0' && args[i].ptr[1] == 'x') {
			args[i].ptr += 2;
			args[i].len -= 2;
			args[i].len = convert16(args[i]);
			args[i].ptr = NULL;
		}
		else {
			args[i].len = convert10(args[i]);
			args[i].ptr = NULL;
		}
	}
}

void apply(char* cmd)
{
	invoke(cmd);
	convert_args();

	if (cmpstr(currcmd.ptr, ".byte", currcmd.len)) {
		for (int i = 0; i < args.size(); i++) {
			*(char*)(&obuf[0] + pos) = (char)args[i].len;
			pos++;
		}
	}
	else if (cmpstr(currcmd.ptr, ".word", currcmd.len)) {
		for (int i = 0; i < args.size(); i++) {
			*(short*)(&obuf[0] + pos) = (short)args[i].len;
			pos += 2;
		}
	}
	else if (cmpstr(currcmd.ptr, ".dword", currcmd.len)) {
		for (int i = 0; i < args.size(); i++) {
			*(int*)(&obuf[0] + pos) = (int)args[i].len;
			pos += 4;
		}
	}
}

void getcmd(void)
{
	args.clear();
	static char cmd[512];
	int i;
	for (i = 0; i < sizeof(cmd); i++) 
		cmd[i] = 0;
	for (i = 0; cmd[i - 1] != '\n'; i++) 
		cmd[i] = getc(stdin);
	cmd[i + 1] = '\n';
	apply(cmd);
}

int main(void)
{
	printf("rasm compiler\n");
	while (true) {
		printf("> ");
		getcmd();
	}
}
