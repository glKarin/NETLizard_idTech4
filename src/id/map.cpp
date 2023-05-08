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
	e.Classname() = "info_player_start";
	e.Name() = "info_player_start_1";
	e("origin", startPos);
	e("angle", startAngle);
	entitys.push_back(e);

	idVec3 center = bounds.Center();
	idVec3 radius = bounds.Size();
	e.ClearSpawnArgs();
	e.Classname() = "light";
	e.Name() = "light_1";
	e("origin", center);
	e("light_radius", radius);
	e("noshadows", true);
	e("nospecular", true);
	e("falloff", 0);
	e("_color", {0.78, 0.78, 0.84});
	entitys.push_back(e);
}
