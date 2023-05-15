#ifndef _KARIN_MAPFILE_H
#define _KARIN_MAPFILE_H

#include <vector>
#include <iostream>
#include <map>

#include "entity.h"
#include "str.h"
#include "bounds.h"

#define MAP_VERSION_DOOM3 "2"
#define MAP_VERSION_QUAKE4 "3"

class idMapFile
{
	public:
		idStr & Version();
		const idStr & Version() const;
		float & StartAngle();
		float StartAngle() const;
		idVec3 & StartPos();
		const idVec3 & StartPos() const;
		idMapFile & operator+=(const idBounds &bounds);
		void AddAreaBounds(int i, const idBounds &b);
		idBounds & AreaBounds(int i);
		idEntity & operator[](int i);
		const idEntity & operator[](int i) const;

		void FillExtras(void);

		idMapFile & operator<<(const idEntity &entity);
		friend std::ostream & operator<<(std::ostream &o, const idMapFile &v);
		
	private:
		idStr version = MAP_VERSION_DOOM3;
		idEntityList entitys;
		idVec3 startPos;
		float startAngle;
		idBounds bounds;
		bool boundsInited = false;
		std::map<int, idBounds> areas;
};

inline idMapFile & idMapFile::operator<<(const idEntity &entity)
{
	entitys.push_back(entity);
	return *this;
}

inline idStr & idMapFile::Version()
{
	return version;
}

inline const idStr & idMapFile::Version() const
{
	return version;
}

inline float & idMapFile::StartAngle()
{
	return startAngle;
}

inline float idMapFile::StartAngle() const
{
	return startAngle;
}

inline idVec3 & idMapFile::StartPos()
{
	return startPos;
}

inline const idVec3 & idMapFile::StartPos() const
{
	return startPos;
}

inline idEntity & idMapFile::operator[](int i)
{
	return entitys[i];
}

inline const idEntity & idMapFile::operator[](int i) const
{
	return entitys[i];
}

inline idBounds & idMapFile::AreaBounds(int i)
{
	return areas[i];
}
#endif
