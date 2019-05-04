#include "rasm.h"

int main()
{
	printf("rasm assembler [ver 0.1]\n\n");
	compile1("test.s", "test.s1");
	printf("\n+++\n\n");
	compile2("test.s1", "test.s2");
	return 0;
}
