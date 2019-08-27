#include <string.h>
#include <string>
#include <vector>

#include <rasm/lex.h>
#include <rasm/file.h>
#include <rasm/errno.h>

// defined in preprosess.cpp
extern int declared(void);

int _next_value(int prev);

static bool is_int(const char * s, int * out)
{
    int k = 1;
    bool neg = false;
    int i = 0;
    int end = strlen(s);

    if(s[0] == '-') {
        i++;
        neg = true;
    }

    if(s[i] == '0' && (s[i + 1] == 'x' || s[i + 1] == 'X')) {
        k *= 16;
        i += 2;
    }
    
    if(k == 1) {
        end--;
        if(s[end] == 'h')
            k *= 16;
        else if(s[end] == 'b')
            k *=2;
        else if(s[end] == 'd')
            k *= 10;
        else {
            k *= 10;
            end++;
        }
    }

    int x = 0;

    while(i < end) {
        char c = s[i];
        if(c - '0' < k)
            x = x * k + c - '0';
        else if(c - 'a' < k)
            x = x * k + c - 'a';
        else if(c - 'A' < k)
            x = x * k + c - 'A';
        else return false;
        i++;
    }
    *out = x;
    return true;
}

static int _not(int prev)
{
    int v = _next_value(0);
    if(v == 0)
        return 1;
    return 0;
}

static int _sum(int prev)
{    
    return prev + _next_value(0);
}

static int _sub(int prev)
{
    return prev - _next_value(0);
}

static int _equal(int prev)
{
    static int _equs = 0;
    _equs++;
    
    int _x = _next_value(prev);
    _equs--;

    if(_equs) {
        if(prev == _x)
            return prev;
        return 0;
    }
    else
        return prev == _x;
}

int _next_value(int prev)
{
    token t = nonspace_token(true);

    if(t.type == TLINE)
        return prev;

    if(t.type == TOP)
        switch(t.op) {
            case '!':
                return _not(prev);
            case '+':
                return _sum(prev);
            case '-':
                return _sub(prev);
            case '=':
                return _equal(prev);
            default:
                warnf("unrecognized operand: '%s'", to_str(t).c_str());
                return 0;
        }
    
    if(t.type || t.str == "")
        errorf("illegal symbol detected: '%s'", to_str(t).c_str());

    if(t.str == "declared")
        return declared();
    else if(is_int(t.str.c_str(), &prev))
        return _next_value(prev);
    else
        errorf("illegal symbol detected: '%s'", to_str(t).c_str());
}

int value(void)
{
    return _next_value(0);
}
