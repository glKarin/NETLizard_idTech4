#ifndef _KARIN_MAP_H
#define _KARIN_MAP_H

#include <vector>
#include <iostream>
#include <map>

#include "vector.h"
#include "plane.h"
#include "str.h"
#include "bounds.h"
#include "dict.h"

#define MAP_VERSION_DOOM3 "2"
#define MAP_VERSION_QUAKE4 "3"

class idBrushDef3Side
{
	public:
		idPlane plane;
		idVec3 textureMatrix[2];
		idStr material;

		idBrushDef3Side()
		{
			textureMatrix[0].Set(1, 0, 0);
			textureMatrix[1].Set(0, 1, 0);
		}
		friend std::ostream & operator<<(std::ostream &o, const idBrushDef3Side &v);
};
using idBrushDef3SideList = std::vector<idBrushDef3Side>;

class idBrushDef3
{
	public:
		idBrushDef3SideList sides;
		friend std::ostream & operator<<(std::ostream &o, const idBrushDef3 &v);
};
using idBrushDef3List = std::vector<idBrushDef3>;

class idEntity
{
	public:
		idStr name;
		idStr classname;
		idBrushDef3List brushs;
		idDict spawnArgs;

		friend std::ostream & operator<<(std::ostream &o, const idEntity &v);
};
using idEntityList = std::vector<idEntity>;

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
