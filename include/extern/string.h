#ifndef _STRING_H_
#define _STRING_H_

#define STRING_SIZE 1024

struct string
{
	char buf[STRING_SIZE];
	size_t len;
	vector<string> v = vector<string>();
	
	string()
	{
		*buf = '\0';
		len = 0;
	}

	string(const char* ptr)
	{
		len = 0;
		while (*ptr != 0) {
			*(buf + len) = *ptr++;
			len++;
		}
	}

	string(char* buf, size_t len)
	{
		if (len != 0)
			memcpy(this->buf, buf, len);
		this->len = len;
	}

	const char* str()
	{
		buf[len] = '\0';
		return buf;
	}

	string operator+ (string s)
	{
		string s1 = string(this->buf, this->len);
		memcpy(s1.buf + s1.len, s.buf, s.len);
		s1.len += s.len;
		return s1;
	}

	string operator+= (string& s)
	{
		string s1 = this->operator+(s);
		memcpy(this->buf, s1.buf, s1.len);
		this->len = s1.len;
		return s1;
	}

	bool operator== (string& s)
	{
		if (s.len != len)
			return false;
		return memcmp(buf, s.buf, len) == 0;
	}

	char& operator[] (size_t i)
	{
		if (i >= len)
			die("String index exceeds its length");
		return buf[i];
	}

	vector<string> split(char symbol = ' ')
	{
		if (len > STRING_SIZE)
			die("String length exceeds max possible value");

		v.erase();
		v = vector<string>();
		if (len == 0) return v;

		char* ptr = buf;
		size_t len = 0;

		for (size_t i = 0; i < this->len; i++) {
			if (buf[i] != symbol) len++;
			else {
				v.add(string(ptr, len));
				while (buf[i] == symbol) i++;
				ptr = buf + i;
				len = 1;
			}
		}
		v.add(string(ptr, len));

		return vector<string>(v.buf, v.len);
	}

	string remove_trash()
	{
		if (len > STRING_SIZE)
			die("String length exceeds max possible value");

		string s = string(this->buf, this->len);
		if (s.len == 0) return s;

		s = s.split('|')[0];
		char* ptr = s.buf;
		while (*ptr == ' ' || *ptr == '\t') {
			ptr++;
			if (s.len != 0)
				s.len--;
			else return s;
			pos++;
		}
		if(s.buf != ptr)
			memcpy(s.buf, ptr, s.len);

		ptr = s.buf + s.len - 1;
		while (*ptr == ' ' || *ptr == '\t') {
			ptr--;
			if (s.len != 0)
				s.len--;
			else return s;
			pos++;
		}
		return s;
	}

	string remove_start(size_t c)
	{
		if (len > STRING_SIZE)
			die("String length exceeds max possible value");
		if (len < c)
			die("String part length exceeds string length");

		string s = string(this->buf, this->len);
		memcpy(s.buf, s.buf + c, s.len);
		s.len -= c;
		return s;
	}

	void append(char c)
	{
		if (len > STRING_SIZE)
			die("String length exceeds max possible value");

		buf[len] = c;
		len++;
	}

	void clear()
	{
		for (size_t i = 0; i < STRING_SIZE; i++)
			buf[i] = 0;
	}
};

void vremove_trash(vector<string>* v);

#endif
