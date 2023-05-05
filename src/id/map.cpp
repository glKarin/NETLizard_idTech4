#include "map.h"

using std::ostream;

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
	e.classname = "info_player_start";
	e.spawnArgs.Set("name", "info_player_start_1");
	e.spawnArgs.SetVec3("origin", startPos);
	e.spawnArgs.SetFloat("angle", startAngle);
	entitys.push_back(e);

	idVec3 center = bounds.Center();
	idVec3 radius = bounds.Size();
	e.spawnArgs.Clear();
	e.classname = "light";
	e.name = "light_1";
	e.spawnArgs.SetVec3("origin", center);
	e.spawnArgs.SetVec3("light_radius", radius);
	e.spawnArgs.SetBool("noshadows", true);
	e.spawnArgs.SetBool("nospecular", true);
	e.spawnArgs.SetFloat("falloff", 0);
	e.spawnArgs.SetVec3("_color", {0.78, 0.78, 0.84});
	entitys.push_back(e);
}
