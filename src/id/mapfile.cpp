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
	areaBounds.insert({i, b});
}

void idMapFile::AddAreaPlane(int i, const idPlane &p)
{
	areaPlanes[i].push_back(p);
}

void idMapFile::AddItemBounds(int i, const idBounds &bv)
{
	itemBounds.push_back(bv);
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

bool idMapFile::IntersectItem(const idBounds &b) const
{
	for(const idBounds &bv : itemBounds)
	{
		if(bv.HasIntersection(b))
			return true;
	}
	return false;
}

bool idMapFile::InArea(const idBounds &b) const
{
	idVec3 points[8];
	b.ToPoints(points);
	for(int i = 0; i < 8; i++)
	{
		int found = -1;
		for(const auto &bv : areaBounds)
		{
		auto str = bv.second.ToString();
			if(bv.second.ContainsPoint(points[i]))
			{
				found = bv.first;
				break;
			}
		}
		if(found < 0)
			return false;

		if(areaPlanes.find(found) == areaPlanes.end())
			continue;

		const auto &ap = (((idMapFile *)this)->areaPlanes)[found];
		bool out = false;
		for(const auto &p : ap)
		{
			if(p.Side(points[i]) < 0.0f)
			{
				out = true;
				break;
			}
		}
		if(out)
			return false;
	}

	return true;
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
