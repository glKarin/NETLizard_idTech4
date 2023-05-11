#include "map.h"

using std::ostream;

idMap & idMap::operator+=(const idBounds &b)
{
	if(!boundsInited)
	{
		bounds = b;
		boundsInited = true;
	}
	else
		bounds += b;
	return *this;
}

void idMap::AddAreaBounds(int i, const idBounds &b)
{
	areas.insert({i, b});
}

ostream & operator<<(ostream &o, const idMap &v)
{
	o << idStr::va("\"Version\" \"%s\"\n", v.version.c_str());
	int i = 0;
	for(const auto &v : v.entitys)
	{
		o << idStr::va("//entity %d\n", i++);
		o << v;
	}

	return o;
}

void idMap::FillExtras(void)
{
	idEntity e;
	e.info_player_start(startPos, startAngle);
	e.Name("info_player_start_1");
	entitys.push_back(e);
	return;

	idVec3 center = bounds.Center();
	idVec3 radius = bounds.Size();
	e.light(center, radius, true, true, {0.78, 0.78, 0.84});
	e.Name("light_1");
	entitys.push_back(e);
}
