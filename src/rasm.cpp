#include <rasm.h>

unsigned line = -1;
unsigned pos = -1;

void die(const char* ptr)
{
	if (line == -1)
		fprintf(stderr, "E: %s\n", ptr);
	else if (pos == -1)
		fprintf(stderr, "%u: %s\n", line, ptr);
	else
		fprintf(stderr, "%u:%u %s\n", line, pos, ptr);
	exit(1);
}
