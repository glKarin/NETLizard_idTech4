#ifndef _KARIN_BRUSH_H
#define _KARIN_BRUSH_H

#include <vector>
#include <iostream>

#include "vector.h"
#include "plane.h"
#include "str.h"

class idDrawVert;

class idBrushDef3Side
{
	public:
		idPlane plane;
		idVec3 textureMatrix[2];
		idStr material;

/* Quake3
		side->texMat[0] = idVec3(0.03125f, 0.0f, 0.0f);
		side->texMat[1] = idVec3(0.0f, 0.03125f, 0.0f);
*/
		idBrushDef3Side()
		{
			textureMatrix[0].Set(1, 0, 0);
			textureMatrix[1].Set(0, 1, 0);
		}
		friend std::ostream & operator<<(std::ostream &o, const idBrushDef3Side &v);

		bool FromDrawVerts(const idDrawVert verts[3]);

		private:
        static bool CalculateFromPoints(idVec3 mat[2], const idVec3 points[3], const idVec2 uvs[3], const idVec3& normal);
};
using idBrushDef3SideList = std::vector<idBrushDef3Side>;

class idBrushDef3
{
	public:
		idBrushDef3SideList sides;
		friend std::ostream & operator<<(std::ostream &o, const idBrushDef3 &v);
};
using idBrushDef3List = std::vector<idBrushDef3>;

#endif
