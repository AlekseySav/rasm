#include <rasm.h>

#define BUF_SIZE 1024

static char buf[BUF_SIZE];
static char* bp = buf;

struct define {
	string name;
	vector<string> args;
	vector<string> code;
};

struct constant {
	string name;
	string value;
};

static bool string_cmp(string s1, string s2)
{
	char* p1 = s1.buf, * p2 = s2.buf;
	if (!is_symbol(*p1)) return false;

	while (p1 - s1.buf <= s1.len && p2 - s2.buf <= s2.len) {
		if (!is_symbol(*p1)) break;
		if (*p1++ != *p2++)
			return false;
	}
	return true;
}

static void compile_command(string name, string args)
{
	static bool isdef = false;
	static vector<define> defines = vector<define>();
	static vector<constant> consts = vector<constant>();
	static vector<bool> ifs;
	static define d;

	string s;
	size_t i;

	if (strcmp(name.str(), ".endif") == 0) {
		if (ifs.len == 0)
			die("No directive \"if\" for endif");
		ifs.pop();
		return;
	}
	if (ifs.len > 0)
		if (!(ifs[ifs.len - 1]))
			return;
	
	if (strcmp(name.str(), ".const") == 0) {
		if (isdef)
			die("Cannot sign constant in define");
		constant c;
		c.name = args.split()[0];
		c.value = args.remove_start(c.name.len).remove_trash();
		consts.add(c);
	}
	else if (strcmp(name.str(), ".define") == 0) {
		if (isdef)
			die("Tried to sign define before previous ended");
		isdef = true;

		s = args.split()[0];	// define name
		for (i = 0; i < defines.len; i++)
			if (defines[i].name == s)
				die("Define declared twice");

		pos += s.len;
		args = args.remove_start(s.len);
		d.name = s;
		d.args = args.split(',');
		vremove_trash(&d.args);
		pos += args.len;

		d.code.erase();
		d.code = vector<string>();
	}
	else if (strcmp(name.str(), ".enddef") == 0) {
		if (!isdef)
			die("No directive \".define\" for \".enddef\"");
		defines.add(d);
		isdef = false;
	}
	else if (strcmp(name.str(), ".ifdef") == 0) {
		for (i = 0; i < defines.len; i++)
			if (defines[i].name == args) {
				ifs.add(true);
				return;
			}
		ifs.add(false);
	}
	else if (strcmp(name.str(), ".ifndef") == 0) {
		for (i = 0; i < defines.len; i++)
			if (defines[i].name == args) {
				ifs.add(false);
				return;
			}
		ifs.add(true);
	}
	else if (isdef)
		d.code.add(name + " " + args.remove_trash() + "\n");
	else {
		for (i = 0; i < defines.len; i++)
			if (defines[i].name == name) {
				d = defines[i];
				vector<string> v = args.split(',');
				for (i = 0; i < v.len; i++)
					v[i] = v[i].remove_trash();

				if (v.len < d.args.len)
					die("To less arguments for define");
				if (v.len > d.args.len)
					die("Trash in the end of line");

				for (i = 0; i < d.code.len; i++) {
					s = d.code[i];
					string out;
					for (size_t j = 0; j < s.len; j++) {
						pos++;
						for (size_t k = 0; k < v.len; k++)
							if (string_cmp(s.buf + j, d.args[k].buf)) {
								out += v[k];
								j += d.args[k].len;
							}
						if (j < s.len)
							out.append(s[j]);
					}

					for (size_t j = 0; j < out.len; j++) {
						if (bp - buf >= BUF_SIZE - 1)
							die("Exceeded max length of command");
						*bp++ = out[j];
					}
				}
				goto add_consts;
			}

		// not defined - copy
		for (i = 0; i < name.len; i++) {
			if (bp - buf >= BUF_SIZE)
				die("Exceeded max length of command");
			*bp++ = name[i];
		}
		*bp++ = ' ';
		for (i = 0; i < args.len; i++) {
			if (bp - buf >= BUF_SIZE)
				die("Exceeded max length of command");
			*bp++ = args[i];
		}
		*bp++ = '\n';

	add_consts:
		s = string(buf, bp - buf);
		bp = buf;
		for (i = 0; i < s.len; i++) {
			for (size_t j = 0; j < consts.len; j++)
				if (string_cmp(s.buf + i, consts[j].name)) {
					for (size_t k = 0; k < consts[j].value.len; k++) {
						*bp++ = consts[j].value[k];
						if (bp - buf >= BUF_SIZE)
							die("Exceeded max length of command");
					}
					i += consts[j].name.len;
					if (i >= s.len) break;
				}
			*bp++ = s[i];
			if (bp - buf >= BUF_SIZE)
				die("Exceeded max length of command");
		}
	}
}

static void compile_line(string s, unsigned ln)
{
	line = ln;
	printf("%s\n", s.str());
	if ((s = s.remove_trash()).len == 0) return;

	string name;

	pos = 0;
	name = s.split()[0];

	pos += name.len;
	s = s.remove_start(name.len);
	s = s.remove_trash();

	compile_command(name, s);
}

void compile1(const char* file, const char* of)
{
	part = 1;
	string s = string();
	char ibuf = '\0';
	unsigned line = 1;

	FILE* f = fopen(file, "rt");
	FILE* f1 = fopen(of, "wt+");
	while (fread(&ibuf, 1, 1, f) != 0) {
		if (ibuf != '\n')
			s.append(ibuf);
		else {
			for (size_t i = 0; i < BUF_SIZE; i++)
				buf[i] = 0;
			compile_line(s, line);
			fwrite(buf, 1, bp - buf, f1);
			bp = buf;
			line++;
			s = "";
		}
	}
	fclose(f);
	fclose(f1);
}
