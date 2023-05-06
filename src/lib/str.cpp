#include "str.h"

#include <stdarg.h>
#include <string.h>
#include <iostream>

using std::string;

#define VA_MAX 10

const char * idStr::va(const char *fmt, ...)
{
	va_list argptr;
	static int index = 0;
	static char string[VA_MAX][16384];	// in case called by nested functions
	char *buf;

	buf = string[index];
	index = (index + 1) % VA_MAX;

	va_start(argptr, fmt);
	vsprintf(buf, fmt, argptr);
	va_end(argptr);

	return buf;
}

bool idStr::GetFilePath(string &path) const
{
	int pos = rfind("/");
	if(pos == string::npos)
		return false;
	path = substr(0, pos);
	return true;
}

bool idStr::GetBaseName(string &ret) const
{
	int pos = rfind("/");
	string path = pos != string::npos ? substr(pos) : *static_cast<const string *>(this);
	if(path.length() == 1)
		return false;
	path = path.substr(1);
	int end = path.rfind(".");
	if(end != string::npos)
		path = path.substr(0, end);
	ret = path;
	return true;
}

bool idStr::GetFileName(string &ret) const
{
	int pos = rfind("/");
	string path = pos != string::npos ? substr(pos) : *static_cast<const string *>(this);
	if(path.length() == 1)
		return false;
	ret = path.substr(1);
	return true;
}

void idStr::ReplaceExtension(const char *ext)
{
	int pos = rfind("/");
	if(pos == length() - 1)
		return;
	int end = rfind(".");
	if(end < pos)
		return;
	if(end == pos + 1)
		return;
	erase(end);
	if(ext)
		append(".").append(ext);
}

void idStr::RemoveExtension()
{
	ReplaceExtension(nullptr);
}

char idStr::Back() const
{
	return empty() ? '\0' : at(length() - 1);
}

idStr operator/(const idStr &a, const idStr &b)
{
	if(a.empty())
		return b;
	else
	{
		idStr res(a);
    	if(res.Back() == '/')
			res.append(b);
		else
			res.append("/").append(b);
		return res;
	}
}

idStr & idStr::operator/=(const idStr &name)
{
	idStr tmp(*this);
	clear();
	append(tmp / name);
	return *this;
}

idStr & idStr::Replace(const char *from, const char *to)
{
	int i;
	int fromLength;
	int toLength;

	fromLength = strlen(from);
	toLength = strlen(to);
	while((i = find(from)) != string::npos)
	{
		replace(i, fromLength, to);
		i += toLength;
	}
	return *this;
}

idStr & idStr::ToLower()
{
	for(auto &ch : *this)
	{
		if(ch >= 'A' && ch <= 'Z')
			ch += 'a' - 'A';
	}
	return *this;
}