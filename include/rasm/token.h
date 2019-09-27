#ifndef _RASM_TOKEN_H_
#define _RASM_TOKEN_H_

typedef struct {
    int type;  
    union {
        buffer * buf;
        char op[3];         // op has only 2 chars (max), op[2] is equal to 0 (need for print_token)
        long long num;      // if type is TINT; if TOP num = op[1] << 8 + op[0]
    };
    struct file_line pos;
} token;

#define TUNDEF     -1   // undefined token
#define TNULL       0
#define TEOF        1
#define TEOL        2
#define TOP         3   // operator
#define TSTR        4   // "str"
#define TCHAR       5   // 'c'
#define TINT        6   // clear number

#define TASM        16  // asm compiler commands
#define TBYTE       (TASM | 1)
#define TWORD       (TASM | 2)
#define TDWORD      (TASM | 3)
#define TQUAD       (TASM | 4)
#define TASCII      (TASM | 5)

#define TPREP       32  // preprocess
#define TMACRO      (TPREP | 1)
#define TEND        (TPREP | 2)
#define TERROR      (TPREP | 3)
#define TWARN       (TPREP | 4)
#define TRELEASE    (TPREP | 5)
#define TINCLUDE    (TPREP | 6)
#define TIF         (TPREP | 7)
#define TELSE       (TPREP | 8)

#define TEXPREP     (TPREP | 16)
#define TDECLARED   (TEXPREP | 1)
#define TTYPEOF     (TEXPREP | 2)

#define tok_release(t) (tok_free(t), ((t) = NULL))

#define tok_op(t, str_op) (t->type == TOP && t->op[0] == str_op[0] && t->op[1] == str_op[1])

token * tok_nil(void);
void tok_free(token * t);
token * tok_copy(token * src);

const char * print_token(token * t);

token * preprocess_token(token * t);
token * read_token(bool preprocess);
void tok_unget1(token * t);

#define NOCVT_TOKEN(t) (t->type == TOP && t->op[0] == '#' && t->op[1] == '#')

#endif
