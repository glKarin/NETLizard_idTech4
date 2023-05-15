#ifndef _KARIN_DEF_H
#define _KARIN_DEF_H

#include <iostream>
#include "list.h"

#include "str.h"
#include "dict.h"

class idDef
{
	public:
		const idStr & Type(void) const;
		const idStr & Name(void) const;
		idDef & Type(const char *type);
		idDef & Name(const char *name);
		idDef & operator()(const char *name, const char *value);
		idDef & operator()(const char *name, float value);
		idDef & operator()(const char *name, int value);
		idDef & operator()(const char *name, bool value);
		idDef & operator()(const char *name, const idVec3 &value);
		idDef & operator()(const char *name, const idMat3 &value);
		friend std::ostream & operator<<(std::ostream &o, const idDef &v);
		idDef & mapDef(const char *map, const char *name = "");

		static const char *DEF_MAP;

	private:
		idStr type;
		idStr name;
		idDict args;
};
using idDefList = idList<idDef>;



inline const idStr & idDef::Type(void) const
{
	return type;
}

inline const idStr & idDef::Name(void) const
{
	return name;
}

inline idDef & idDef::Type(const char *type)
{
	this->type = type;
	return *this;
}

inline idDef & idDef::Name(const char *name)
{
	this->name = name;
	return *this;
}

inline idDef & idDef::operator()(const char *name, const char *value)
{
	args.Set(name, value);
	return *this;
}

inline idDef & idDef::operator()(const char *name, float value)
{
	args.SetFloat(name, value);
	return *this;
}

inline idDef & idDef::operator()(const char *name, int value)
{
	args.SetInteger(name, value);
	return *this;
}

inline idDef & idDef::operator()(const char *name, bool value)
{
	args.SetBool(name, value);
	return *this;
}

inline idDef & idDef::operator()(const char *name, const idVec3 &value)
{
	args.SetVec3(name, value);
	return *this;
}

inline idDef & idDef::operator()(const char *name, const idMat3 &value)
{
	args.SetMat3(name, value);
	return *this;
}

#endif
