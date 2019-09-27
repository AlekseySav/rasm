#ifndef _RASM_PARSE_H_
#define _RASM_PARSE_H_

#define ASM_NIL 0
#define ASM_REG 1
#define ASM_MEM 2
#define ASM_IMM 3
#define ASM_TOP 4

#define asm_op(num, op) (num == (op[1] << 8 + op[0]))

typedef struct {
    int type;
    long long value;
} asm_int;

asm_int parse_tokens(void);

#endif
 