#ifndef _RASM_PREPROCESS_H_
#define _RASM_PREPROCESS_H_

void pr_init(void);
void pr_shutdown(void);

token * token_buffer(void);
void preprocess(token * t);

bool is_defined(token * t);
bool is_arg(token * t);

#endif
