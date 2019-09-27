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

static const char * stdinc_file = "rasm.s";

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
    if(!FLAG_CHECK(_nostdinc))
        rf_open(stdinc_file);

    token * t;
    token * prev = NULL;
    size_t isasm = 0; // is asm command
    size_t pos = 0;

    do {
        while((t = read_token(true))->type != TUNDEF) {
            if(t->type & TPREP) {
                preprocess(t);
                continue;
            }
            else if(t->type == TOP && t->op[0] == ':' && t->op[1] == '\0') {
                tok_free(t);
                t = tok_nil();
                t->type = TINT;
                t->num = pos;
                define_const(prev, t);
                prev = NULL;
                continue;
            }
            
            if(prev && FLAG_CHECK(_E)) {
                if(prev->type == TEOL) printf("\n");
                else printf("%s ", print_token(prev));
            }

            if(t->type & TASM) {
                if(isasm) warnf(t->pos.name, t->pos.row, t->pos.col, "expected (new-line) symbol");
                switch(t->type) {
                    case TASCII:
                    case TBYTE: isasm = 1; break;
                    case TWORD: isasm = 2; break;
                    case TDWORD: isasm = 4; break;
                    case TQUAD: isasm = 8; break;
                    default:
                        warnf(t->pos.name, t->pos.row, t->pos.col, "unknown assembler command");
                }
            }
            else if(t->type == TEOL)
                isasm = 0;
            else if(t->type == TSTR)
                pos += isasm * buf_len(t->buf);
            else if(t->type != TOP)   // for each element in .byte a1, a2, ...
                pos += isasm;
            tok_release(prev);
            prev = t;

            if(t->type == TEOF)
                break;
        }
        tok_release(prev);
        if(t->type == TUNDEF)
            errorf(t->pos.name, t->pos.row, t->pos.col, "token %s is undefined", print_token(t));
    } while(rf_close());
    tok_release(t);
    
    printf("\n");

    pr_shutdown();
    rf_shutdown();
    return 0;
}
