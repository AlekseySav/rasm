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
    else if(t->type == TINT)
        t->num = src->num;
    t->pos = src->pos;
    return t;
}

static const char * tok_undef = "{???}";
static const char * tok_null = "(null)";
static const char * tok_eof = "(eof)";
static const char * tok_eol = "(new-line)";
static char tok_num[50];

static const char * tok_asm[] = { ".byte", ".word", ".dword", ".quad", ".ascii" };
static const char * tok_prep[] = { ".macro", ".end", ".error", ".warning", 
    ".release", ".include", ".if", ".else" };
static const char * tok_exprep[] = { "declared", "typeof" };

#define ARRSIZ(l) (sizeof(l) / sizeof((l)[0]))

const char * print_token(token * t)
{
    if(t->type & TEXPREP)
        return tok_exprep[t->type - TEXPREP - 1];
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
        case TINT:
            sprintf(tok_num, "%#llx", t->num);
            return tok_num;
        default:
            return tok_undef;
    }
}

struct optional_long {
    bool is_value;
    long long value;
};

static struct optional_long is_int(const char * s)
{
    unsigned k = 0;
    bool neg = false;
    int i = 0;
    struct optional_long res = { false, 0 };

    int end = strlen(s);

    if(s[0] == '-') {
        i++;
        neg = true;
    }

    if(s[i] == '0' && (s[i + 1] == 'x' || s[i + 1] == 'X')) {
        k = 16;
        i += 2;
    }
    
    if(!k) {
        end--;
        if(s[end] == 'h')
            k = 16;
        else if(s[end] == 'b')
            k = 2;
        else if(s[end] == 'd')
            k = 10;
        else {
            k = 10;
            end++;
        }
    }

    if(i >= end) return res;    // no symbols except attributes

    while(i < end) {
        unsigned char c = s[i];
        if(c - '0' < k)
            res.value = res.value * k + c - '0';
        else if(c - 'a' + 10 < k)
            res.value = res.value * k + c - 'a' + 10;
        else if(c - 'A' + 10 < k)
            res.value = res.value * k + c - 'A' + 10;
        else return res;    // res.is_value is still false
        i++;
    }
    if(neg)
        res.value = -res.value;
    res.is_value = true;
    return res;
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

token * preprocess_token(token * t)
{
    if(is_defined(t))
        return read_token(preprocess);
    return t;
}

static token * unget;

void tok_unget1(token * t)
{
    unget = t;
}

token * read_token(bool preprocess)
{
    char c;
    token * t;
    
    if(unget != NULL) {
        t = unget;
        unget = NULL;
        return t;
    }

    t = token_buffer();
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
        int i;

        if(s[0] == '.') {
            for(i = 0; i < ARRSIZ(tok_asm); i++)              // mb asm
                if(strcmp(s, tok_asm[i]) == 0) {
                    t->type = TASM + i + 1;
                    break;
                }

            if(!t->type)
                for(i = 0; i < ARRSIZ(tok_prep); i++)          // mb preprosess
                    if(strcmp(s, tok_prep[i]) == 0) {
                        t->type = TPREP + i + 1;
                        break;
                    }

            if(t->type)
                buf_release(t->buf);
        }

        for(i = 0; i < ARRSIZ(tok_exprep); i++)              // mb exprep
            if(strcmp(s, tok_exprep[i]) == 0) {
                t->type = TEXPREP + i + 1;
                break;
            }

        if(!t->type) {
            struct optional_long l = is_int(s);
            if(l.is_value) {
                buf_free(t->buf);
                t->num = l.value;
                t->type = TINT;
            }
        }

        if(!t->type)
            if(preprocess)
                return preprocess_token(t);
    }
    return t;
}
