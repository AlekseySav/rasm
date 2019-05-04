#include "rasm.h"

string input = "";
string out = "a.out";

int main(int argc, char ** argv)
{
	for(int i = 0; i < argc; i++) {
		if(strcmp(argv[i], "-a") == 0 || strcmp(argv[i], "--allflags") == 0)
			flags = ~0;
		else if(strcmp(argv[i], "-n") == 0 || strcmp(argv[i], "--noremove") == 0)
			flags |= FLAG_NOCLEAR;
		else if(strcmp(argv[i], "-l") == 0 || strcmp(argv[i], "--consolelog") == 0)
			flags |= FLAG_LOG;
		else if(strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--version") == 0)
			flags |= FLAG_VERSION;
		else if(strcmp(argv[i], "-o") == 0 && i != argc - 1)
			out = argv[++i];
		else
			input = argv[i];
	}

	if(flags & FLAG_VERSION)
		printf("rasm assembler [ver 0.1]\n\n");

	if(input.len == 0)
		die("No input files");

	compile1(input.str(), (input + "1").str());
	
	if(flags & FLAG_LOG)
		printf("\n+++\n\n");

	compile2((input + "1").str(), (input + "2").str());
	
	if(flags & FLAG_LOG)
		printf("\n+++\n\n");
	compile3((input + "2").str(), out.str());

	if(!(flags & FLAG_NOCLEAR)) {
		remove((input + "1").str());
		remove((input + "2").str());
	}

	return 0;
}
