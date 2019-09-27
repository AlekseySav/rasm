#include <stddef.h>
#include <stdlib.h>

#include <bool.h>
#include <buffer.h>
#include <error.h>
#include <vector.h>

#include <rasm/file.h>
#include <rasm/token.h>
#include <rasm/preprocess.h>
#include <rasm/parse.h>

enum node_id {
    INT,
    OP_NEG
};

typedef struct node {
    struct node * left, * right;
    enum node_id id;
    asm_int src;
} node;

static node * node_create(node src)
{
    node * n = malloc(sizeof(node));
    n->left = src.left;
    n->right = src.right;
    n->id = src.id;
    n->src = src.src;
    return n;
}

static inline void node_free(node * n)
{
    free(n);
}

static char to_char(token * t, size_t * pos)
{
    const char * err = "invalid character format";
    const char * s = buf_cstr(t->buf);
    size_t len = buf_len(t->buf) - *pos;

    if(len == 0)
        errorf(t->pos.name, t->pos.row, t->pos.col, err);

    if(s[(*pos)++] != '\\') return s[*pos - 1];
    
    // '\X'
    if(len == 1)
        errorf(t->pos.name, t->pos.row, t->pos.col, err);

    if(len == 2 && s[*pos] >= '0' && s[*pos] <= '7')
        return s[(*pos)++] - '0';
    switch(s[(*pos)++]) {
        case 'a': return '\a';
        case 'b': return '\b';
        case 'e': return '\e';
        case 'f': return '\f';
        case 'n': return '\n';
        case 'r': return '\r';
        case 't': return '\t';
        case 'v': return '\v';
        default: (*pos)--;
    }
    
    // '\033'
    if(len < 4)
        errorf(t->pos.name, t->pos.row, t->pos.col, err);

    if(s[(*pos)++] == '0' && s[(*pos)++] == '3' && s[(*pos)++] == '3') // ESC
        return '\033';
    errorf(t->pos.name, t->pos.row, t->pos.col, err);
}

static asm_int parse_next(asm_int prev)
{
    static int int_type = ASM_MEM;
    static token * t;
    
    tok_free(t);
    t = read_token(true);
    size_t pos = 0; // for to_char

    if(t->type == TINT) {
        return (asm_int) { int_type, t->num };
        int_type = ASM_MEM;
    }
    else if(t->type == TCHAR)
        return (asm_int) { ASM_IMM, to_char(t, &pos) };
    if(t->type == TOP) {
        if(tok_op(t, "#")) {
            int_type = ASM_IMM;
            return parse_next(prev);
        }
        else if(tok_op(t, "!"))
            return (asm_int) { prev.type, !parse_next(prev).value };
        else if(tok_op(t, "&"))
            return (asm_int) { prev.type, prev.value & parse_next(prev).value };
        else if(tok_op(t, "|"))
            return (asm_int) { prev.type, prev.value | parse_next(prev).value };
        else if(tok_op(t, "+"))
            return (asm_int) { prev.type, prev.value + parse_next(prev).value };
        else if(tok_op(t, "-"))
            return (asm_int) { prev.type, prev.value - parse_next(prev).value };
        else if(tok_op(t, "*"))
            return (asm_int) { prev.type, prev.value * parse_next(prev).value };
        else if(tok_op(t, "/"))
            return (asm_int) { prev.type, prev.value / parse_next(prev).value };
        else if(tok_op(t, "<<"))
            return (asm_int) { prev.type, prev.value << parse_next(prev).value };
        else if(tok_op(t, ">>"))
            return (asm_int) { prev.type, prev.value >> parse_next(prev).value };
        else if(tok_op(t, "=="))
            return (asm_int) { prev.type, prev.value == parse_next(prev).value };
        else if(tok_op(t, "!="))
            return (asm_int) { prev.type, prev.value != parse_next(prev).value };
    }
    if(t->type & TPREP)
        switch(t->type) {
            case TDECLARED:
                free(t);
                t = read_token(false);
                return (asm_int) { ASM_IMM, is_macro(t) };
            case TTYPEOF:
                return (asm_int) { ASM_IMM, parse_next(prev).type };
        }
    if(t->type == TEOF)
        errorf(t->pos.name, t->pos.row, t->pos.col, "unexpected %s", print_token(t));

    tok_unget1(t);
    return (asm_int) { 0 };
}

static asm_int parse_block(void)
{
    asm_int i = (asm_int) { 0 };
    asm_int prev = (asm_int) { 0 };
    while((i = parse_next(prev)).type)
        prev = i;
    return prev;
}

asm_int parse_tokens(void)
{
    
    return parse_block();
}
