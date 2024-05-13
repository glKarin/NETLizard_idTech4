#include "mapfile.h"

using std::ostream;

idMapFile::idMapFile(const char *version)
	: version(version)
{
}

idMapFile & idMapFile::operator+=(const idBounds &b)
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

void idMapFile::AddAreaBounds(int i, const idBounds &b)
{
	areas.insert({i, b});
}

ostream & operator<<(ostream &o, const idMapFile &v)
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

#if 0
void idMapFile::FillExtras(void)
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
#endif
