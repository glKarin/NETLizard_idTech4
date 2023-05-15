#include "def.h"

const char *idDef::DEF_MAP = "mapDef";

std::ostream & operator<<(std::ostream &o, const idDef &v)
{
	o << v.type << " " << v.name << " {\n";
	if(!v.args.IsEmpty())
		o << v.args;
	o << "}\n";
	return o;
}

idDef & idDef::mapDef(const char *map, const char *name)
{
	Type(DEF_MAP);
	Name(map);
	args.Set("name", name);
	args.Set("devname", map);
	return *this;
}
