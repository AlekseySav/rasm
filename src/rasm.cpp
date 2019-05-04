#include <rasm.h>

unsigned line = -1;
unsigned pos = -1;
unsigned part = -1;

int flags = 0;

void die(const char* ptr)
{
	if(part == -1)
		fprintf(stderr, "E: %s\n", ptr);
	else if (line == -1)
		fprintf(stderr, "%u: %s\n", part, ptr);
	else if (pos == -1)
		fprintf(stderr, "%u:%u %s\n", part, line, ptr);
	else
		fprintf(stderr, "%u:%u:%u %s\n", part, line, pos, ptr);
	exit(1);
}
