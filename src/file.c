#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <buffer.h>
#include <error.h>
#include <flags.h>
#include <vector.h>

#include <rasm/file.h>

struct file {
    FILE * ptr;
    buffer * name;
    buffer * dir;
    int r, c;
    char prev;
};

static vector * files;                  // vector<file>
static vector * paths;                  // vector<const char *>

#define curr_file ((struct file *)vec_tail(files))

static const char * path_null = "";
static const char * path_this = "./";
static const char * path_rasm = "rasm/";

struct file_line rf_getline(void)
{
    struct file_line l;
    l.name = buf_cstr(curr_file->name);
    l.row = curr_file->r;
    l.col = curr_file->c;
    return l;
}

void rf_setline(struct file_line nl)
{
    curr_file->name = buf_create(nl.name);
    curr_file->r = nl.row;
    curr_file->c = nl.col;
}

void rf_init(void)
{
    files = vec_nil();
    paths = vec_nil();

    add_path(path_null);
    add_path(path_this);
    if(!FLAG_CHECK(_nostdinc))
        add_path(path_rasm);
}

void rf_shutdown(void)
{
    while(vec_len(files))
        rf_close();
    vec_release(files);
}

void add_path(const char * inc)
{
    vec_push(paths, inc);
}

void rf_open(const char * file)
{
    struct file * f = calloc(1, sizeof(struct file));
    assert(f);

    buffer * b;
    if(vec_len(files))
        vec_push(paths, buf_cstr(curr_file->dir));      // add current dir

    for(size_t i = 0; i < vec_len(paths); i++) {
        b = buf_create((const char *)vec_get(paths, i));
        buf_append(b, file);

        f->ptr = fopen(buf_cstr(b), "rt");
        buf_release(b);
        
        if(f->ptr)
            break;
    }

    if(vec_len(files))
        vec_pop(paths);

    if(!f->ptr) {
        if(vec_len(files) == 0)
            panic("cannot open input file");
    }

    f->r++;
    f->name = buf_create(file);
    f->dir = buf_create(file);
    while(buf_len(f->dir) && buf_pop(f->dir) != '/');  // remove short-name
    buf_push(f->dir, '/');

    vec_push(files, f);
}

void rf_close(void)
{
    struct file * f = (struct file *)vec_pop(files);
    buf_free(f->name);
    buf_free(f->dir);
    free(f);
}

#define is_space(c) (c == ' ' || c == '\t' || c == '\v' || c == '\f')

char rf_getc(void)
{
    char c;
    if(curr_file->prev == EOF)
        return EOF;
    
    c = (char)getc(curr_file->ptr);

    if(is_space(c) && is_space(curr_file->prev))
        c = rf_getc();          // nonspace char

    if(c == ';')
        while((c = (char)getc(curr_file->ptr)) != '\n');    // skip comment

    if(c == '\n') {
        curr_file->c = 0;
        curr_file->r++;
    }
    else curr_file->c++;

    curr_file->prev = c;
    return c;
}

void rf_ungetc(char c)
{
    if(c == EOF || curr_file->prev == EOF)
        return;

    ungetc((int)c, curr_file->ptr);
    if(c == '\n') {
        curr_file->c = 1;
        curr_file->r--;
    }
    else curr_file->c--;
}
