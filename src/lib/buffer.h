#ifndef _KARIN_BUFFER_H
#define _KARIN_BUFFER_H

class idBuffer
{
	public:
		~idBuffer() {
			Free();
		}
		void Alloc(int size);
		void Free();
		int file_get_contents(const char *name);
		operator const char *() const {
			return data;
		}
		operator char *() {
			return data;
		}
		char & operator[](int i) {
			return data[i];
		}
		char operator[](int i) const {
			return data[i];
		}
		int Size() const {
			return size;
		}
		const char * Data() const {
			return data;
		}

	private:
		char *data = nullptr;
		int size = 0;
};

#endif
