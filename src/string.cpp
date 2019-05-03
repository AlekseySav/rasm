#include <rasm.h>

void vremove_trash(vector<string>* v)
{
	if (v->len == 0) return;

	for (size_t i = 0; i < v->len; i++)
		v->operator[](i) = v->operator[](i).remove_trash();
}
