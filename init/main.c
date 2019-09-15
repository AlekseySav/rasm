#include <stddef.h>
#include <string.h>
#include <stdio.h>

#include <bool.h>
#include <buffer.h>
#include <error.h>
#include <flags.h>
#include <vector.h>

#include <rasm/file.h>
#include <rasm/token.h>
#include <rasm/preprocess.h>

static void usage(void)
{
    die("usage: %s [options] <file>\n"
        "\n"
        "options:\n"
        "  --help           display options on screen\n"
        "  -E               print preprocessed code\n"
        "  -I <path>        add to include path\n"
        "  -nostdinc        disable standard rasm directives\n"
        "  -Wall            enable all warnings\n"
        "  -Werror          make all warnings into errors\n", getprogname());
}

int main(int argc, char * argv[])
{
    setprogname(argv[0]);
    rf_init();
    pr_init();

    char * file = NULL;
    
    for(int i = 1; i < argc; i++) {
        if(strcmp(argv[i], "--help") == 0)
            usage();
        else if(strcmp(argv[i], "-E") == 0)
            FLAG_ADD(_E);
        else if(strcmp(argv[i], "-I") == 0)
            add_path(argv[++i]);
        else if(strcmp(argv[i], "-nostdinc") == 0)
            FLAG_ADD(_nostdinc);
        else if(strcmp(argv[i], "-Wall") == 0)
            FLAG_ADD(_Wall);
        else if(strcmp(argv[i], "-Werror") == 0)
            FLAG_ADD(_Werror);
        else {
            if(file)
                panic("too much input files");
            else file = argv[i];
        }
    }

    if(!file)
        panic("no input files");

    rf_open(file);

    token * t;

    while((t = read_token(true))->type != TEOF) {
        if(t->type & TPREP)
            preprocess(t);
        else if(FLAG_CHECK(_E)) {
            if(t->type == TEOL) printf("\n");
            else printf("%s ", print_token(t));
        }
        tok_release(t);
    }
    tok_release(t);

    rf_close();

    pr_shutdown();
    rf_shutdown();
    return 0;
}
