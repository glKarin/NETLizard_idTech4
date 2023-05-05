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
		idStr version = MAP_VERSION_DOOM3;
		idEntityList entitys;
		idVec3 startPos;
		float startAngle;
		idBounds bounds;
		std::map<int, idBounds> areas;

		void FillExtras(void);

		friend std::ostream & operator<<(std::ostream &o, const idMap &v);
};

#endif
