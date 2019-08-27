#ifndef _LEX_H_
#define _LEX_H_

using std::string;

#define TNULL   0
#define TEOF    1
#define TLINE   2
#define TSPACE  3
#define TCOMMA  4

#define TOP     5   // operand
#define TSTR    6

#define TPREP   16  // preprosess
#define TCONST      (1  | TPREP)
#define TMACRO      (2  | TPREP)
#define TIF         (3  | TPREP)
#define TELSE       (4  | TPREP)
#define TEND        (5  | TPREP)
#define TRELEASE    (6  | TPREP)
#define TINCLUDE    (7  | TPREP)
#define TWARNING    (8  | TPREP)
#define TERROR      (9  | TPREP)

struct token {
    int type;
    char op;
    string str;
};

token read_token(bool convert);
token nonspace_token(bool convert);

void preprosess_command(token t);

#endif
