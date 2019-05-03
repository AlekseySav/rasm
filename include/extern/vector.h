#ifndef _VECTOR_H_
#define _VECTOR_H_

#define VECTOR_SIZE 128

template<typename Ty>
struct vector
{
	Ty* buf;
	size_t len;

	vector(Ty* buf, size_t len, bool create_buf = false)
	{
		if (len > VECTOR_SIZE)
			die("Vector length exceeds max possible value");
		if (buf == NULL && create_buf)
			die("Tried to copy vector buffer from NULL");

		this->len = len;
		if (!create_buf)
			this->buf = buf;
		else {
			this->buf = (Ty*)malloc(VECTOR_SIZE * sizeof(Ty));
			if (this->buf == NULL)
				die("Not enough memory enabled");
			memcpy(this->buf, buf, this->len);
		}
	}

	vector()
	{
		buf = (Ty*)malloc(VECTOR_SIZE * sizeof(Ty));
		len = 0;
	}

	void erase()
	{
		if (buf == NULL)
			die("Tried to delete NULL value");
		free(buf);
		len = 0;
	}

	Ty& operator[](size_t i)
	{
		if (i >= len)
			die("Vector index exceeds its length");
		return buf[i];
	}

	void add(Ty t)
	{
		if (len >= VECTOR_SIZE)
			die("Vector length exceeds max possible value");
		buf[len] = t;
		len++;
	}

	Ty pop()
	{
		if (len == 0)
			die("Tried to pop element from null-vector");
		len--;
		return buf[len];
	}

	size_t index(Ty t)
	{
		for (size_t i = 0; i < len; i++)
			if (buf[i] == t)
				return i;
		return -1;
	}

};

#endif
