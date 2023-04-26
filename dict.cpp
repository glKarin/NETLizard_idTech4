#include "dict.h"

#include <algorithm>
#include "vector.h"
#include "matrix.h"

std::ostream & operator<<(std::ostream &o, const idDict &v)
{
	for(const auto &name : v.sequence)
	{
		o << idStr::va("\"%s\" \"%s\"\n", name.c_str(), v.hash.at(name).c_str());
	}
	return o;
}

bool idDict::Contains(const char *name) const
{
	return std::find(sequence.cbegin(), sequence.cend(), name) != sequence.cend();
}

void idDict::Set(const char *name, const char *value)
{
	if(!Contains(name))
		sequence.push_back(name);
	hash.insert({name, value});
}

void idDict::SetFloat(const char *name, float value)
{
	Set(name, idStr::va("%f", value));
}

void idDict::SetInteger(const char *name, int value)
{
	Set(name, idStr::va("%i", value));
}

void idDict::SetBool(const char *name, bool value)
{
	Set(name, idStr::va("%i", value));
}

void idDict::SetVec3(const char *name, const idVec3 &value)
{
	Set(name, idStr::va("%f %f %f", value[0], value[1], value[2]));
}

const char * idDict::GetString(const char *name, const char *def) const
{
	if(!Contains(name))
		return def;
	return hash.at(name);
}

void idDict::SetMat3(const char *name, const idMat3 &value)
{
	Set(name, idStr::va("%f %f %f %f %f %f %f %f %f", value[0], value[1], value[2], value[3], value[4], value[5], value[6], value[7], value[8]));
}

