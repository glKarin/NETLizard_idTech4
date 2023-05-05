#include "buffer.h"

#include <stdio.h>

void idBuffer::Alloc(int size)
{
	Free();
	data = new char[size];
	this->size = size;
}

void idBuffer::Free()
{
	delete[] data;
	data = nullptr;
	size = 0;
}

int idBuffer::file_get_contents(const char *name)
{
	FILE *file;
	long l;
	size_t ret;

	Free();

	file = fopen(name, "rb");
	if(!file)
		return -1;
	fseek(file, 0, SEEK_END);
	l = ftell(file);
	Alloc((int)l);
	fseek(file, 0, SEEK_SET);
	ret = fread(data, 1, size, file);
	fclose(file);

	return ret;
}
