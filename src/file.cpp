#include <stdio.h>
#include <string.h>
#include <vector>
#include <string>

using std::vector;
using std::string;

#include <rasm/lex.h>
#include <rasm/file.h>
#include <rasm/errno.h>

typedef struct {
    char _name[128]; // null if from tokens
    FILE * _file;
    vector<token> _src;
    int _r, _c;
    char _prev;
    bool _macro;
    int _col;
} file;

struct {
    int line, col;
    char name[128];
} file_pos = { 0 };

static vector<file> opened_files;
#define curr_file opened_files.back()

int & line(void)
{
    return file_pos.line;
}

int & column(void)
{
    return file_pos.col;
}

const char * filename(void)
{
    if(!file_pos.name[0])
        return NULL;
    return file_pos.name;
}

void append_tokens(vector<token> tokens, bool is_macro)
{
    size_t tsz = tokens.size();
    if(tsz == 0)
        return;

    token t;

    for(size_t i = 0; i < tsz / 2; i++) {
        t = tokens[i];
        tokens[i] = tokens[tsz - i - 1];
        tokens[tsz - i - 1] = t;
    }

    file f = { 0 };
    f._src = tokens;
    f._macro = is_macro;
    opened_files.push_back(f);
}

//defined in preprosess.cpp
extern void kill_macro(void);

token pop_token(void)
{
    if(curr_file._name[0])
        return { 0 };

    token t = curr_file._src.back();
    curr_file._src.pop_back();
    
    if(!curr_file._src.size()) {
        if(curr_file._macro)
            kill_macro();
        opened_files.pop_back();
    }
    
    return t;
}

void ropen(const char * filename)
{
    file f = { 0 };
    f._file = fopen(filename, "rt");
    if(!f._file) {
        if(opened_files.size())
            errorf("cannot open %s source file", filename);
        else
            panic((string("cannot open input file ") + filename).c_str());
    }

    f._r = 1;
    strcpy(f._name, filename);
    opened_files.push_back(f);

    strcpy(file_pos.name, f._name);
    file_pos.line = 1;
    file_pos.col = 0;
}

int rclose(void)
{
    if(!opened_files.size())
        return 0;
    
    opened_files.pop_back();
    if(opened_files.size()) {
        size_t i = opened_files.size();
        while(!opened_files[--i]._name[0]);
        strcpy(file_pos.name, opened_files[i]._name);
        file_pos.line = opened_files[i]._r;
        file_pos.col = opened_files[i]._c;
    }

    return opened_files.size();
}

char rgetc(void)
{
    if(!opened_files.size())
        panic("no input files found");
    
    if(curr_file._prev == EOF)
        return EOF;

    char c = getc(curr_file._file);

    if(c == EOF && curr_file._prev != '\n') {
        curr_file._prev = EOF;
        return '\n';
    }

    if(c == '\t' || c == '\v' || c == '\f')
        c = ' ';

    if(c == ' ' && curr_file._prev == ' ')
        return rgetc();

    else if(c == '|')
        while((c = getc(curr_file._file)) != '\n');

    if(c == '\n') {
        curr_file._r++;
        curr_file._col = curr_file._c;
        curr_file._c = 0;

        file_pos.line++;
        file_pos.col = 0;
    }

    curr_file._prev = c;
    curr_file._c++;

    file_pos.col++;

    return c;
}

void rungetc(char c)
{
    if(c == EOF)
        return;
    
    ungetc(c, curr_file._file);

    if(c == '\n') {
        curr_file._r--;
        curr_file._c = curr_file._col;

        file_pos.line--;
        file_pos.col = curr_file._col;
    }
    else {
        curr_file._c--;
        file_pos.col--;
    }

    curr_file._prev = 0;
    curr_file._col = -1;

}
