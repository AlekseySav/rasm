#ifndef _FILE_H_
#define _FILE_H_

int & line(void);
int & column(void);
const char * filename(void);

// add token array as file
void append_tokens(std::vector<token> tokens, bool is_macro);
token pop_token(void);

void ropen(const char * filename);
int rclose(void);   // returns opened files count

char rgetc(void);
void rungetc(char c);

#endif
