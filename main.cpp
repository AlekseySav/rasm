#include "vector.h"
#include "string.h"

typedef struct command {
    string_t name;
    vector_t<string_t> args;
} command_t;

typedef struct define {
    string_t name;
    vector_t<string_t> args;
    vector_t<command_t> src;
} define_t;


typedef unsigned char byte;
typedef unsigned short word;
byte buf[512];
void * pos = buf;


#define NEG 1
#define HEX 2
#define OCT 4
#define BIN 8

int toint(string_t s)
{
    char * ptr = s.buf;
    size_t len = s.len;

	int flag = 0;

	if (*ptr == '-') {
		flag |= NEG;
		ptr++;
		len--;
	}

	if (*ptr == '0') {
		flag |= OCT;
		ptr++;
		len--;
		if (*ptr == 'x' || *ptr == 'X') {
			flag |= HEX;
			ptr++;
			len--;
		}
	}

	if (*(ptr + len - 1) == 'b' && !(flag & HEX)) {
		flag |= BIN;
		len--;
	}

	int i = 0;

	while (len--) {
		if (flag & BIN) i = i * 2 + *ptr - '0';
		else if (flag & HEX) {
			i = i * 16 + *ptr;
			if (*ptr >= '0' && *ptr <= '9')
				i -= '0' ;
			else if (*ptr >= 'a' && *ptr <= 'z')
				i -= 'a' - 10;
			else
				i -= 'A' - 10;
		}
		else if (flag & OCT)
			i = i * 8 + *ptr - '0';
		else // DEC
			i = i * 10 + *ptr - '0';
		ptr++;
	}

	if (flag & NEG) i = -i;
	return i;
}

void compile_command(command_t c)
{
    static bool is_define = false;
    static vector_t<define_t> defines = vector_t<define_t>();
    static define_t curr;

    vector_t<string_t> v;
    if(strcmp(c.name.str(), ".define") == 0) {
        if(is_define)
            die("Must end previous define before declare new");
        is_define = true;
        curr.args = vector_t<string_t>();
        curr.src  = vector_t<command_t>();
        v = c.args[0].split();
        curr.name = v[0];
        if(v.len > 1) {
            curr.args.append(v[1]);
            if(v.len > 2)
                die("Trash at the end if line");
        }
        for(size_t i = 1; i < c.args.len; i++)
            curr.args.append(c.args[i]);
    }
    else if(strcmp(c.name.str(), ".enddef") == 0) {
        if(!is_define)
            die("No define signed");
        is_define = false;
        defines.append(curr);
    }
    else if(strcmp(c.name.str(), ".const") == 0) {
        if(is_define)
            die("const can't be signed in define");
        v = c.name.split();
        curr.name = v[1];
        curr.src = vector_t<command_t>();
        curr.args = vector_t<string_t>();
        curr.src.append({ v[2], vector_t<string_t>() });
        defines.append(curr);
    }
    else if(is_define) {
        curr.src.append(c);
    }
    else if(c.name.buf[0] == '.') {
        if(strcmp(c.name.str(), ".byte")) {
            for(size_t i = 0; i < c.args.len; i++)
                *(byte *)pos = (byte)toint(c.args[i]);
        }
        else if(strcmp(c.name.str(), ".word")) {
            for(size_t i = 0; i < c.args.len; i++)
                *(word *)pos = (word)toint(c.args[i]);
        }
    }
}

void compile_line(string_t s, unsigned line)
{
    printf("%u: %s\n", line + 1, s.str());
    vector_t<string_t> v;
    v = s.remove_trash().split();
    command_t c;
    c.name = v[0].remove_trash();

    s = s.remove_start(c.name.len);
    v = s.split(',');
    for(size_t i = 0; i < v.len; i++)
        v.set(i, v[i].remove_trash());
    c.args = v;
    compile_command(c);
}

void compile(const char * ptr)
{
    vector_t<string_t> v;
    v = string_t(ptr).split('\n');
    for(size_t i = 0; i < v.len; i++)
        compile_line(v[i], i);
}

int main(void)
{
    printf("\n");
    compile(".define mov x, y\n\t.word 0xfeeb\n.enddef\nmov");
}
