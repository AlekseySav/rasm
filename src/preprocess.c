#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <bool.h>
#include <error.h>
#include <buffer.h>
#include <vector.h>

#include <rasm/file.h>
#include <rasm/token.h>

typedef struct {
    buffer * name;
    size_t split;           // arg_split[split - 1] macro_arg arg_split[split]
    vector * value;         // vector<token>; NULL if not optional
} macro_arg;

typedef struct {
    buffer * name;
    vector * arg_split;     // vector<token>
    vector * args;          // vector<macro_arg>
    vector * src;           // vector<token>
} macro;

typedef struct {
    size_t pos;
    vector * src;           // pointer to macro->src
    bool close_macro;
} macro_code;

typedef struct {
    vector * keys;          // vector<const char *>
    vector * src;           // vector<vector<token>>
} dict;

static vector * macros;         // vector<macro>
static vector * opened_macros;  // vector<macro_code>
static vector * opened_args;    // vector<dict>

static macro_arg * arg_create(macro_arg arg)
{
    macro_arg * m = malloc(sizeof(macro_arg));
    m->name = arg.name;
    m->split = arg.split;
    m->value = arg.value;
    return m;
}

static void arg_free(macro_arg * m)
{
    buf_release(m->name);
    if(m->value) {
        for(size_t i = 0; i < vec_len(m->value); i++)
            tok_free((token *)vec_get(m->value, i));
        vec_release(m->value);
    }
    free(m);
}

static macro * mac_nil(void)
{
    macro * m = malloc(sizeof(macro));
    m->arg_split = vec_nil();
    m->args = vec_nil();
    m->src = vec_nil();
    return m;
}

static void mac_free(macro * m)
{
    size_t i;
    for(i = 0; i < vec_len(m->arg_split); i++)
        tok_free((token *)vec_get(m->arg_split, i));
    for(i = 0; i < vec_len(m->args); i++)
        arg_free((macro_arg *)vec_get(m->args, i));
    for(i = 0; i < vec_len(m->src); i++)
        tok_free((token *)vec_get(m->src, i));
    buf_release(m->name);
    vec_release(m->arg_split);
    vec_release(m->args);
    vec_release(m->src);
}

static macro_code * code_from_buffer(vector * v)
{
    macro_code * c = malloc(sizeof(macro_code));
    c->src = v;
    c->pos = 0;
    return c;
}

static macro_code * code_create(macro * m)
{
    macro_code * c = code_from_buffer(m->src);
    c->close_macro = true;
    return c;
}

static void code_free(macro_code * c)
{
    free(c);
}

static dict * dict_create(vector * k, vector * v)
{
    dict * d = malloc(sizeof(dict));
    d->keys = k;
    d->src = v;
}

static void dict_free(dict * d)
{
    vec_free(d->keys);
    for(size_t i = 0; i < vec_len(d->src); i++) {
        vector * tokens = (vector *)vec_get(d->src, i);
        for(size_t j = 0; j < vec_len(tokens); j++)
            tok_free((token *)vec_get(tokens, j));
        vec_free(tokens);
    }
    vec_free(d->src);
    free(d);
}

static const void * dict_get(dict * d, const char * key)
{
    for(size_t i = 0; i < vec_len(d->keys); i++)
        if(strcmp((const char *)vec_get(d->keys, i), key) == 0)
            return vec_get(d->src, i);

    return NULL;
}

void pr_init(void)
{
    macros = vec_nil();
    opened_macros = vec_nil();
    opened_args = vec_nil();
}

void pr_shutdown(void)
{
    if(macros == NULL) return;

    size_t i;

    for(i = 0; i < vec_len(macros); i++)
        mac_free((macro *)vec_get(macros, i));
    for(i = 0; i < vec_len(opened_macros); i++)
        code_free((macro_code *)vec_get(opened_macros, i));
    for(i = 0; i < vec_len(opened_args); i++)
        dict_free((dict *)vec_get(opened_args, i));
    vec_release(macros);
    vec_release(opened_macros);
    vec_release(opened_args);
}

token * token_buffer(void)
{
    if(vec_len(opened_macros)) {
        macro_code * c = (macro_code *)vec_tail(opened_macros);
        if(c->pos == vec_len(c->src)) {
            if(c->close_macro)
                dict_free((dict *)vec_pop(opened_args));
            code_free((macro_code *)vec_pop(opened_macros));    // delete 'c'
            return token_buffer();
        }
        token * t = (token *)vec_get(c->src, c->pos);
        return tok_copy((token *)vec_get(c->src, c->pos++));
    }
    return NULL;
}

#define op_equal(t1, t2) ((t1->type == TEOL && t2->type == TEOL) || \
    ((t1)->op[0] == (t2)->op[0] && (t1)->op[1] == (t2)->op[1]))

static void read_args(macro * m)    // buf is vector<token>
{
    vector * fmt = m->arg_split;

    vector * args = vec_nil();  // vector<vector<token>>
    vector * tochar = vec_nil();    // vector<const char *>
    dict * d;

    if(vec_len(fmt)) {
        size_t split = 0;
        vector * v = vec_nil();     // vector<token>
        token * t, * f;
        macro_arg * ma;

        while((t = read_token(true))->type != TEOF) {
            
            if(t->type != TNULL) {
                f = (token *)vec_get(fmt, split);
                if(op_equal(t, f)) {
                    split++;
                    if(v->len) {
                        if(vec_len(m->args) <= vec_len(args))
                            errorf(t->pos.name, t->pos.row, t->pos.col, "too much arguments");
                        ma = (macro_arg *)vec_get(m->args, vec_len(args));

                        vec_push(args, v);
                        v = vec_nil();
                    }
                    tok_free(t);
                    if(vec_len(fmt) == split) 
                        break;
                    continue;
                }
            }
            vec_push(v, t);
        }
    
        for(size_t i = 0; i < vec_len(m->args); i++)
            vec_push(tochar, buf_cstr(((macro_arg *)vec_get(m->args, i))->name));

        if(vec_len(tochar) != vec_len(args))
            errorf(t->pos.name, t->pos.row, t->pos.col, "invalid arguments count");
    }

    d = dict_create(tochar, args);
    vec_push(opened_args, d);
}

static void read_macro(void)
{
    token * t;
    macro * m = mac_nil();

    // read name
    t = read_token(true);
    if(t->type != TNULL)
        errorf(t->pos.name, t->pos.row, t->pos.col, "%s is uncorrect macro name", print_token(t));
    m->name = t->buf;
    free(t);

    // read args
    size_t s = 1;
    macro_arg arg = { 0 };
    while((t = read_token(true))->type != TEOF) {
        if(t->type == TNULL) {
            if(arg.name == NULL) {
                arg.name = t->buf;
                arg.split = s;
                free(t);
            }
            else errorf(t->pos.name, t->pos.row, t->pos.col, "illegal symbol founded: '%s'", print_token(t));
        }
        //else if(t->type == TOP && t->op[0] == '-' && t->op[1] == '>')
        //    arg.value = read_token(true);
        else {
            if(t->type != TOP && t->type != TEOL)
                errorf(t->pos.name, t->pos.row, t->pos.col, "illegal symbol founded: %s", print_token(t));
            vec_push(m->arg_split, t);
            if(arg.name) {
                vec_push(m->args, arg_create(arg));
                arg.name = NULL;
                arg.value = NULL;
            }
            s++;
        }
        if(t->type == TEOL)
            break;
    }

    if(s - arg.split >= 2)
        vec_pop(m->arg_split);      // rm \n

    // read body
    bool mbend = false;
    while((t = read_token(false))->type != TEOF) {
        if(t->type == TEND)
            mbend = true;
        else if(mbend) {
            if(strcmp(buf_cstr(t->buf), buf_cstr(m->name)) == 0)
                break;      // .end macro
            mbend = false;
        }
        
        vec_push(m->src, t);
    }
    if(t->type == TEOF)
        errorf(t->pos.name, t->pos.row, t->pos.col, "missed '.end' for '%s' macro", buf_cstr(m->name));

    vec_pop(m->src);        // .end
    if(vec_len(m->src) && ((token *)vec_tail(m->src))->type == TEOL)
        vec_pop(m->src);    // '\n'

    vec_push(macros, m);
}

void preprocess(token * t)
{
    switch(t->type) {
        case TMACRO:
            read_macro();
            break;
        case TEND:
            warnf(t->pos.name, t->pos.row, t->pos.col, "no '.macro' or '.if' for '.end' command");
            break;
        default:
            warnf(t->pos.name, t->pos.row, t->pos.col, "%s: unrecognized preprocessor directive", print_token(t));
            break;
    }
}

bool is_defined(token * t)
{
    size_t i;
    macro * m;
    const char * s = buf_cstr(t->buf);

    for(i = 0; i < vec_len(macros); i++) {
        m = (macro *)vec_get(macros, i);
        if(strcmp(buf_cstr(m->name), s) == 0) {
            read_args(m);
            vec_push(opened_macros, code_create(m));
            return true;
        }
    }
    return false;
}

bool is_arg(token * t)
{
    const char * s = buf_cstr(t->buf);

    for(size_t i = 0; i < vec_len(opened_args); i++) {
        vector * v = (vector *)dict_get((dict *)vec_get(opened_args, i), s);
        if(v) {
            macro_code * c = code_from_buffer(v);
            c->close_macro = false;
            vec_push(opened_macros, c);
            return true;
        }
    }
    return false;
}
