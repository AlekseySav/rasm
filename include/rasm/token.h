#ifndef _RASM_TOKEN_H_
#define _RASM_TOKEN_H_

typedef struct {
    int type;  
    buffer * buf;
    long value;
    char op[3];     // op has only 2 chars (max), op[2] is equal to 0 (need for print_token)
    struct file_line pos;
} token;

#define TUNDEF     -1   // undefined token
#define TNULL       0
#define TEOF        1
#define TEOL        2
#define TOP         3   // operator
#define TSTR        4   // "str"
#define TCHAR       5   // 'c'

token * tok_nil(void);
void tok_release(token * t);

const char * print_token(token * t);
token * read_token(void);

#endif
