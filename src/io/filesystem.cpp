#include "filesystem.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "str.h"

long idFileSystem::FileSize(const char *name)
{
	FILE *file;

	file = fopen(name, "rb");
	if(!file)
		return -1;
	fseek(file, 0, SEEK_END);
	long l = ftell(file);
	fclose(file);
	return l;
}

void idFileSystem::mkdir_p(const char *path)
{
#ifdef __WIN32__
	idStr p("mkdir ");
	p += idStr(path).Replace("/", "\\");
#else
	idStr p("mkdir -p ");
	p /= path;
#endif
	system(p.c_str());
}

bool idFileSystem::FileExists(const char *path)
{
	FILE *file;

	file = fopen(path, "r");
	if(!file)
		return false;
	fclose(file);
	return true;
}
