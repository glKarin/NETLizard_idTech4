#include "map.h"

using std::ostream;

ostream & operator<<(ostream &o, const idBrushDef3Side &v)
{
	return o << idStr::va("( %f %f %f %f ) ( ( %f %f %f ) ( %f %f %f ) ) \"%s\" 0 0 0\n",
			v.plane[0], v.plane[1], v.plane[2], v.plane[3],
			v.textureMatrix[0][0], v.textureMatrix[0][1], v.textureMatrix[0][2],
			v.textureMatrix[1][0], v.textureMatrix[1][1], v.textureMatrix[1][2],
			v.material.c_str());
}

ostream & operator<<(ostream &o, const idBrushDef3 &v)
{
	o << "brushDef3\n";
	o << "{\n";
	for(const auto &v : v.sides)
		o << v;
	o << "}\n";
	return o;
}

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
