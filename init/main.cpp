#include <stdio.h>
#include <string.h>
#include <vector>
#include <string>

#include <rasm/lex.h>
#include <rasm/file.h>
#include <rasm/flags.h>
#include <rasm/errno.h>

void preprosess()
{
    token t = { 0 };

    bool _nr = true;

    while(t.type != TEOF) {
        t = read_token(true);
        if(t.type & TPREP)
            preprosess_command(t);
        else if(t.type == TLINE) {
            if(!_nr)
                printf("\n");
            _nr = true;
        }
        else if(t.type == TSPACE) {
            if(!_nr)
                printf(" ");
        }
        else if(t.type == TEOF && rclose())
            t.type = 0;
        else {
            printf("%s", to_str(t).c_str());
            _nr = false;
        }
    }
    printf("\n");
}

_Noreturn static void usage(int code)
{
    fprintf(code ? stderr : stdout, 
    "usage: rasm [options] file...\n"
    "\n"
    "options:\n"
    "  --help               display options on screen\n"
    "  -o <file>            set the output file\n"
    "  -Wall                enable all warnings\n"
    "  -Werror              make all warnings like if they were errors\n"
    );

    exit(code);
}   

int main(int argc, char ** argv)
{
    flags = 0;
    char * inf = NULL, * outf = NULL;
    for(int i = 1; i < argc; i++) {
        if(argv[i][0] != '-') {
            if(inf == NULL)
                inf = argv[i];
            else
                panic("to much input files");
        }
        else {
            if(strcmp(argv[i], "--help") == 0)
                usage(0);
            else if(strcmp(argv[i], "-Wall") == 0)
                flags |= _Wall;
            else if(strcmp(argv[i], "-Werror") == 0)
                flags |= _Werror;
            else if(strcmp(argv[i], "-o") == 0) {
                if(outf == NULL)
                    outf = argv[++i];
                else
                    panic("to much output files");
            }
            else
                panic("unrecognized command");
        }
    }
    if(inf == NULL)
        panic("no input files");

    ropen(inf);
    preprosess();
    
    return 0;
}
