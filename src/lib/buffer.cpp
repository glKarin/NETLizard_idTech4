#include "buffer.h"

#include <stdio.h>
#include <string.h>

idBuffer::idBuffer(const idBuffer &other)
{
	size = other.size;
	data = nullptr;
	if(other.data)
	{
		Alloc(size, other.data);
	}
}

idBuffer::idBuffer(idBuffer &&other)
{
	size = other.size;
	data = other.data;
	other.size = 0;
	other.data = nullptr;
}

void idBuffer::Alloc(int size, const char *d)
{
	Free();
	data = new char[size];
	this->size = size;
	if(d)
	{
		memcpy(data, d, size);
	}
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

int idBuffer::file_put_contents(const char *name) const
{
	FILE *file;
	long l;
	size_t ret;

	file = fopen(name, "wb");
	if(!file)
		return -1;
	ret = fwrite(data, 1, size, file);
	fflush(file);
	fclose(file);

	return ret;
}

idBuffer & idBuffer::operator=(const idBuffer &other)
{
	size = other.size;
	data = nullptr;
	if(other.data)
	{
		Alloc(size, other.data);
	}
	return *this;
}

idBuffer & idBuffer::operator=(idBuffer &&other)
{
	size = other.size;
	data = other.data;
	other.size = 0;
	other.data = nullptr;
	return *this;
}
