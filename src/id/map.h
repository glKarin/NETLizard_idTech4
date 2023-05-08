#ifndef _KARIN_MAP_H
#define _KARIN_MAP_H

#include <vector>
#include <iostream>
#include <map>

#include "entity.h"
#include "str.h"
#include "bounds.h"

#define MAP_VERSION_DOOM3 "2"
#define MAP_VERSION_QUAKE4 "3"

class idMap
{
	public:
		idStr & Version();
		const idStr & Version() const;
		float & StartAngle();
		float StartAngle() const;
		idVec3 & StartPos();
		const idVec3 & StartPos() const;
		idMap & operator+=(const idBounds &bounds);
		void AddAreaBounds(int i, const idBounds &b);
		idBounds & AreaBounds(int i);
		idEntity & operator[](int i);
		const idEntity & operator[](int i) const;

		void FillExtras(void);

		idMap & operator<<(const idEntity &entity);
		friend std::ostream & operator<<(std::ostream &o, const idMap &v);
		
	private:
		idStr version = MAP_VERSION_DOOM3;
		idEntityList entitys;
		idVec3 startPos;
		float startAngle;
		idBounds bounds;
		bool boundsInited = false;
		std::map<int, idBounds> areas;
};

inline idMap & idMap::operator<<(const idEntity &entity)
{
	entitys.push_back(entity);
	return *this;
}

inline idStr & idMap::Version()
{
	return version;
}

inline const idStr & idMap::Version() const
{
	return version;
}

inline float & idMap::StartAngle()
{
	return startAngle;
}

inline float idMap::StartAngle() const
{
	return startAngle;
}

inline idVec3 & idMap::StartPos()
{
	return startPos;
}

inline const idVec3 & idMap::StartPos() const
{
	return startPos;
}

inline idEntity & idMap::operator[](int i)
{
	return entitys[i];
}

inline const idEntity & idMap::operator[](int i) const
{
	return entitys[i];
}

inline idBounds & idMap::AreaBounds(int i)
{
	return areas[i];
}
#endif
