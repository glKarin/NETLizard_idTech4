#ifndef _KARIN_FILESYSTEM_H
#define _KARIN_FILESYSTEM_H

#include <fstream>

class idStr;

class idFileSystem
{
	public:
		static long FileSize(const char *name);
		static void mkdir_p(const char *path);
		static bool FileExists(const char *path);
		template <class T>
		static int Output(const char *path, const T &object);
};



template <class T>
int idFileSystem::Output(const char *path, const T &object)
{
	std::ofstream os(path);
	os << object;
	os.flush();
	os.close();
	return FileSize(path);
}
#endif
