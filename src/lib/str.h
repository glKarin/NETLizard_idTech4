#ifndef _KARIN_STR_H
#define _KARIN_STR_H

#include <string>

class idStr : public std::string
{
	public:
	using std::string::string;
	using std::string::operator=;
	idStr(const idStr &other) = default;
	idStr(idStr &&other) = default;
	idStr & operator=(const idStr &other) = default;
	idStr & operator=(idStr &&other) = default;
	void ReplaceExtension(const char *ext);
	friend idStr operator/(const idStr &a, const idStr &b);
	idStr & operator/=(const idStr &a);
	char Back() const;
	idStr & Replace(const char *from, const char *to);
	idStr & ToLower();
	void RemoveExtension();

	static const char * va(const char *fmt, ...);

	bool GetFilePath(std::string &path) const;
	bool GetBaseName(std::string &ret) const;
	bool GetFileName(std::string &ret) const;
	operator const char *() const {
		return c_str();
	}
};

#endif