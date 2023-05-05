#include "entity.h"

using std::ostream;

ostream & operator<<(ostream &o, const idEntity &v)
{
	o << "{\n";
	o << idStr::va("\"classname\" \"%s\"\n", v.classname.c_str());
	o << idStr::va("\"name\" \"%s\"\n", v.name.c_str());
	if(!v.spawnArgs.IsEmpty())
		o << v.spawnArgs;

	if(!v.brushs.empty())
	{
		int i = 0;
		for(const auto &v : v.brushs)
		{
			o << idStr::va("//primitive %d\n", i++);
			o << "{\n";
			o << v;
			o << "}\n";
		}
	}
	o << "}\n";
	return o;
}
