#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <bool.h>
#include <buffer.h>
#include <error.h>
#include <rasm/file.h>
#include <rasm/token.h>
#include <rasm/preprocess.h>

token * tok_nil(void)
{
    token * t = calloc(1, sizeof(token));
    return t;
}

void tok_free(token * t)
{
    if(t == NULL)
        return;

    if(t->type == TNULL || t->type == TSTR)
        buf_release(t->buf);        
    free(t);
}

token * tok_copy(token * src)
{
    token * t = tok_nil();
    t->type = src->type;
    if(t->type == TSTR || t->type == TCHAR || t->type == TNULL) {
        if(src->buf)
            t->buf = buf_copy(src->buf);
    }
    else if(t->type == TOP) {
        t->op[0] = src->op[0];
        t->op[1] = src->op[1];
    }
    t->pos = src->pos;
    return t;
}

static const char * tok_undef = "{???}";
static const char * tok_null = "(null)";
static const char * tok_eof = "(eof)";
static const char * tok_eol = "(new-line)";

static const char * tok_asm[] = { ".byte", ".word", ".dword", ".quad" };
static const char * tok_prep[] = { ".macro", ".end", ".error", ".warning", ".release", ".include" };

const char * print_token(token * t)
{
    if(t->type & TASM)
        return tok_asm[t->type - TASM - 1];
    if(t->type & TPREP)
        return tok_prep[t->type - TPREP - 1];
    
    switch(t->type) {
        case TNULL:
            if(t->buf == NULL)
                return tok_null;
            else return buf_cstr(t->buf);
        case TEOF:
            return tok_eof;
        case TEOL:
            return tok_eol;
        case TOP:
            return t->op;   // t->op[2] must be zero
        case TSTR:
        case TCHAR:
            return buf_cstr(t->buf);
        default:
            return tok_undef;
    }
}

static bool is_op(char c) 
{
    return (c == '=' || c == '!' || 
        c == '*' || c == '/' || c == '+' || c == '-' || 
        c == '(' || c == ')' || c == '[' || c == ']' || 
        c == '{' || c == '}' || c == '#' || c == '$' || 
        c == '&' || c == '|' || c == '<' || c == '>' || 
        c == '%' || c == ',' || c == ':');
}

static inline bool is_char(char c)
{
    return (c >= '0' && c <= '9') ||
        (c >= 'a' && c <= 'z') ||
        (c == '_' || c == '.');
}

static char to_char(token * t)
{
    const char * err = "invalid character format";
    const char * s = buf_cstr(t->buf);
    size_t l = buf_len(t->buf);

    if(l == 1) return s[0];
    if(l == 2) { // '\X'
        if(s[0] != '\\')
            errorf(t->pos.name, t->pos.row, t->pos.col, err);
        if(s[1] >= '0' && s[1] <= '7')
            return s[1] - '0';
        switch(s[1]) {
            case 'a': return '\a';
            case 'b': return '\b';
            case 'e': return '\e';
            case 'f': return '\f';
            case 'n': return '\n';
            case 'r': return '\r';
            case 't': return '\t';
            case 'v': return '\v';
        }
    }
    if(l == 4 && s[0] == '\\' && s[1] == '0' && s[2] == '3' && s[3] == '3') // ESC
        return '\033';
    else
        errorf(t->pos.name, t->pos.row, t->pos.col, err);
}

token * read_token(bool preprocess)
{
    char c;
    token * t = token_buffer();
    if(t == NULL) {
        t = tok_nil();

        while((c = rf_getc()) == ' ');

        t->pos = rf_getline();

        if(c == EOF) t->type = TEOF;
        else if(c == '\n') t->type = TEOL;
        else if(c == '\'' || c == '"')
            t->type = TSTR;
        else if(is_op(c)) {
            t->type = TOP;
            t->op[0] = c;
            c = rf_getc();
            if(is_op(c))
                t->op[1] = c;
            else
                rf_ungetc(c);
        }

        if(t->type == TSTR) {
            t->buf = buf_nil();
            char e = c;
            while((c = rf_getc()) != EOF) {
                if(c == e)
                    break;
                buf_push(t->buf, c);
            }
            if(c == EOF)
                errorf(t->pos.name, t->pos.row, t->pos.col, "expected closing comma");
            
            if(c == '\'')
                t->type = TCHAR;
        }

        else if(t->type == TNULL) {
            t->buf = buf_nil();
            while(is_char(c)) {
                buf_push(t->buf, c);
                c = rf_getc();
            }
            rf_ungetc(c);
        }
    }
    
    if(NOCVT_TOKEN(t)) {
        tok_free(t);
        return read_token(false);
    }
    
    if(t->type == TNULL) {
        if(is_arg(t))
            return read_token(preprocess);
            
        const char * s = buf_cstr(t->buf);
        if(strcmp(s, tok_asm[0]) == 0)
            t->type = TBYTE;
        else if(strcmp(s, tok_asm[1]) == 0)
            t->type = TWORD;
        else if(strcmp(s, tok_asm[2]) == 0)
            t->type = TDWORD;
        else if(strcmp(s, tok_asm[3]) == 0)
            t->type = TQUAD;

        else if(strcmp(s, tok_prep[0]) == 0)
            t->type = TMACRO;
        else if(strcmp(s, tok_prep[1]) == 0)
            t->type = TEND;
        else if(strcmp(s, tok_prep[2]) == 0)
            t->type = TERROR;
        else if(strcmp(s, tok_prep[3]) == 0)
            t->type = TWARN;
        else if(strcmp(s, tok_prep[4]) == 0)
            t->type = TRELEASE;
        else if(strcmp(s, tok_prep[5]) == 0)
            t->type = TINCLUDE;
                
        if(preprocess) {
            if(t->type != TNULL)
                buf_release(t->buf);
            else if(is_defined(t))
                return read_token(preprocess);
        }
    }
    return t;
}
