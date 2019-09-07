#ifndef _RASM_FILE_H_
#define _RASM_FILE_H_

struct file_line {
    const char * name;
    int row, col;
};

struct file_line rf_getline(void);
void rf_setline(struct file_line nl);

void rf_init(void);
void rf_shutdown(void);

void add_path(const char * inc);

void rf_open(const char * file);
void rf_close(void);

char rf_getc(void);
void rf_ungetc(char c);

#endif
