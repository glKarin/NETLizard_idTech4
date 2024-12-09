#ifndef _KARIN_BUFFER_H
#define _KARIN_BUFFER_H

class idBuffer
{
	public:
		idBuffer() = default;
		idBuffer(const idBuffer &other);
		idBuffer(idBuffer &&other);
		~idBuffer();
		void Alloc(int size, const char *data = nullptr);
		void Free();
		int file_get_contents(const char *name);
		int file_put_contents(const char *name) const;
		operator const char *() const;
		char & operator[](int i);
		char operator[](int i) const;
		int Size() const;
		const char * Data() const;
		idBuffer & operator=(const idBuffer &other);
		idBuffer & operator=(idBuffer &&other);

	private:
		char *data = nullptr;
		int size = 0;
};

inline idBuffer::~idBuffer() {
	Free();
}

inline idBuffer::operator const char *() const
{
	return data;
}

inline char & idBuffer::operator[](int i)
{
	return data[i];
}

inline char idBuffer::operator[](int i) const
{
	return data[i];
}

inline int idBuffer::Size() const
{
	return size;
}

inline const char * idBuffer::Data() const
{
	return data;
}

#endif
