#include <string>
#include <vector>
#include <map>

#include <rasm/lex.h>
#include <rasm/errno.h>
#include <rasm/file.h>

using std::string;
using std::vector;
using std::map;

struct macro {      // constants are also macros
    string name;
    vector<string> args;
    vector<token> src;
};

static vector<macro> macros;
static vector<map<string, vector<token>>> opened_args;

// defined in value.cpp
int value(void);

static void read_const(void)
{
    macro m;
    token t;
    t = nonspace_token(true);
    if(t.str == "")
        errorf("'%s' is uncorrect name for const", to_str(t).c_str());
    m.name = t.str;
    m.args = { };
    m.src = { };
    while((t = read_token(true)).type != TLINE)
        m.src.push_back(t);
    macros.push_back(m);
}

static void read_macro(void)
{
    macro m;
    token t;
    t = nonspace_token(true);
    if(t.str == "")
        errorf("'%s' is uncorrect name for macro", to_str(t).c_str());
    m.name = t.str;
    m.args = { };
    m.src = { };
    while((t = nonspace_token(true)).type != TLINE) {
        if(t.type == TCOMMA)
            continue;
        if(t.str == "")
            errorf("'%s' is uncorrect name for macro argument", to_str(t).c_str());
        m.args.push_back(t.str);
    }

    bool maybe_end = false;

    while((t = read_token(true)).type != TEOF) {
        if(maybe_end) {
            if(t.type != TSPACE) {
                if(t.str == m.name)
                    break;
                else
                    maybe_end = false;
            }
        }
        if(t.type == TEND)
            maybe_end = true;
        m.src.push_back(t);
    }

    if(t.type == TEOF)
        warnf("no '.end' directive for '%s' macro", m.name.c_str());
    else        // pop .end token
        while(m.src.back().type != TEND)
            m.src.pop_back();
    m.src.pop_back();

    macros.push_back(m);
}

static void do_release(void)
{
    token t = nonspace_token(false);

    if(t.str == "")
        errorf("uncorrect macro name: '%s'", to_str(t).c_str());
        
    for(size_t i = 0; i < macros.size(); i++) {
        macro m = macros[i];
        if(t.str == m.name) {       // delete macros[i]
            for(size_t j = i; j < macros.size() - 1; j++)
                macros[j] = macros[j + 1];
            macros.pop_back();
            return;
        }
    }
    
    warnf("unrecognized macro name: '%s'", to_str(t).c_str());
}

static int endif_count = 0;
static void if_block(bool autoskip)
{
    bool skip;
    if(autoskip)
        skip = true;
    else
        skip = !value();

    token t = { 0 };

    int cnt = 1;
    bool maybe_end = false;

    if(skip)
        while(cnt && (t = read_token(true)).type != TEOF) {
            if(cnt == 1 && t.type == TELSE)
                break;
            if(t.type == TEND)
                maybe_end = true;
            else if(maybe_end && t.type != TSPACE) {
                if(t.str == "if")
                    cnt--;
                maybe_end = false;
            }
        }
    
    if(t.type == TEOF)
        warnf("no '.end if' directive for '.if'");

    if(t.type == TELSE || t.type == TNULL)  // .end if didn't read
        endif_count++;    
}

void preprosess_command(token t)
{
    switch(t.type) {
        case TCONST:
            read_const();
            break;
        case TMACRO:
            read_macro();
            break;
        case TIF:
            if_block(false);
            break;
        case TELSE:
            if(endif_count)
                if_block(true);
            else
                warnf("no '.if' directive for '.else'");
            break;
        case TEND:
            t = nonspace_token(false);
            if(t.str != "if")
                warnf("no '.macro' directive for '.end %s'", to_str(t).c_str());
            else {
                if(endif_count)
                    endif_count--;
                else
                    warnf("no '.if' directive for '.end if'");
            }
            break;
        case TRELEASE:
            do_release();
            break;
        case TINCLUDE:
            t = nonspace_token(true);
            ropen(t.str.c_str());
            break;
        case TWARNING:
            warnf("%s", nonspace_token(true).str.c_str());
            break;
        case TERROR:
            errorf("%s", nonspace_token(true).str.c_str());
            break;
        default:
            warnf("undefined preprosessor command: '%s'", to_str(t).c_str());
            break;
    }
}

bool is_arg(string s)
{
    vector<token> v;
    for(size_t i = 0; i < opened_args.size(); i++) {
        v = opened_args[i][s];
        if(v.size()) {      // not null vector
            append_tokens(v, false);
            return true;
        }
    }
    return false;
}

bool is_macro(string s)
{
    for(size_t i = 0; i < macros.size(); i++) {
        macro m = macros[i];
        
        if(s == m.name) {
            if(m.args.size()) {
                token t = { 0 };
                map<string, vector<token>> args;
                size_t i = 0;

                while((t = nonspace_token(true)).type != TLINE) {
                    vector<token> v;

                    while(t.type != TCOMMA && t.type != TLINE) {
                        v.push_back(t);
                        t = read_token(true);
                    }
                    if(i >= m.args.size())
                        errorf("too much arguments for '%s'", m.name.c_str());
                    args[m.args[i++]] = v;

                    if(t.type == TLINE)
                        break;
                }
                if(i != m.args.size())
                    errorf("too less arguments for '%s'", m.name.c_str());

                opened_args.push_back(args);
                append_tokens(m.src, true);
            }
            else
                append_tokens(m.src, false);

            return true;
        }
    }
    return false;
}

//used in file.cpp
void kill_macro(void)
{
    if(opened_args.size())
        opened_args.pop_back();
}

//used in value.cpp
int declared(void)
{
    token t = nonspace_token(false);
    if(t.str == "") {
        warnf("uncorrect macro name");
        return 0;
    }
    for(size_t i = 0; i < macros.size(); i++)
        if(macros[i].name == t.str)
            return 1;

    return 0;
}
