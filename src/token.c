#include <stdlib.h>
#include <stdio.h>

#include <bool.h>
#include <buffer.h>
#include <error.h>
#include <rasm/file.h>
#include <rasm/token.h>

token * tok_nil(void)
{
    token * t = calloc(1, sizeof(token));
    return t;
}

void tok_release(token * t)
{
    if(t == NULL)
        return;

    if(t->type == TNULL || t->type == TSTR)
        buf_release(t->buf);        
    free(t);
}

static const char * print_undef = "{???}";
static const char * print_null = "(null)";
static const char * print_eof = "(eof)";
static const char * print_eol = "(new-line)";

const char * print_token(token * t)
{
    switch(t->type) {
        case TNULL:
            if(t->buf == NULL)
                return print_null;
            else return buf_cstr(t->buf);
        case TEOF:
            return print_eof;
        case TEOL:
            return print_eol;
        case TOP:
            t->op[2] = 0;
            return t->op;
        case TSTR:
        case TCHAR:
            return buf_cstr(t->buf);
        default:
            return print_undef;
    }
}

static bool is_op(char c) 
{
    return (c == '=' || c == '!' || 
        c == '*' || c == '/' || c == '+' || c == '-' || 
        c == '(' || c == ')' || c == '[' || c == ']' || 
        c == '{' || c == '}' || c == '#' || c == '$' || 
        c == '&' || c == '|' || c == '<' || c == '>' || 
        c == '%' || c == ',');
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

token * read_token(void)
{
    char c;
    token * t = tok_nil();

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
        else {
            t->op[1] = '\0';
            rf_ungetc(c);
        }
    }

    if(t->type == TSTR) {
        t->buf = buf_nil();
        char e = c;
        while(c != EOF) {
            buf_push(t->buf, c);
            if((c = rf_getc()) == e) break;
        }
        if(c == EOF)
            errorf(t->pos.name, t->pos.row, t->pos.col, "expected closing comma");
        buf_push(t->buf, c);
        
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
    return t;
}
