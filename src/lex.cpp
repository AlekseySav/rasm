#include <string>
#include <vector>

#include <rasm/lex.h>
#include <rasm/file.h>
#include <rasm/errno.h>

#define _is_char(c) ( \
    ((c) >= '0' && (c) <= '9') || \
    ((c) >= 'A' && (c) <= 'Z') || \
    ((c) >= 'a' && (c) <= 'z') || \
    ((c) == '.' || (c) == '_'))

// defined in preprosess.cpp
extern bool is_macro(std::string);
extern bool is_arg(std::string);

token read_token(bool convert)
{
    token t = pop_token();

    if(t.type == TNULL && t.str == "") {
        t = { 0 };
        char c = rgetc();
        if(c == EOF)
            t.type = TEOF;
        else if(c == '\n')
            t.type = TLINE;
        else if(c == ' ')
            t.type = TSPACE;
        else if(c == ',')
            t.type = TCOMMA;
        else if(c == '"')
            t.type = TSTR;
        else if(!_is_char(c)) {
            t.type = TOP;
            t.op = c;
        }

        if(t.type && t.type != TSTR)
            return t;

        std::string s = "";

        if(t.type == TSTR) {
            while(c != '\n') {
                c = rgetc();
                if(c == '"')
                    break;
                s.push_back(c);
            }
            if(c == '\n')
                errorf("missing closing quote '\"'");
            t.str = s;
        }
        else {
            do {
                s.push_back(c);
                c = rgetc();
            } while(_is_char(c));

            rungetc(c);

            if(s == ".const")
                t.type = TCONST;
            else if(s == ".macro")
                t.type = TMACRO;
            else if(s == ".if")
                t.type = TIF;
            else if(s == ".else")
                t.type = TELSE;
            else if(s == ".end")
                t.type = TEND;
            else if(s == ".release")
                t.type = TRELEASE;
            else if(s == ".include")
                t.type = TINCLUDE;
            else if(s == ".warning")
                t.type = TWARNING;
            else if(s == ".error")
                t.type = TERROR;
            else
                t.str = s;
        }
    }

    if(t.str != "" && convert && t.type == TNULL)
        if(is_arg(t.str) || is_macro(t.str))
            return read_token(convert);    // next token

    return t;
}

token nonspace_token(bool convert)
{
    token t;
    while((t = read_token(convert)).type == TSPACE);
    return t;
}
