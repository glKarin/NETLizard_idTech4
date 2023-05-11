#ifndef _KARIN_BRUSH_H
#define _KARIN_BRUSH_H

#include <vector>
#include <iostream>

#include "vector.h"
#include "plane.h"
#include "str.h"

class idDrawVert;
class idEntity;
class idVec3;
class idBounds;

class idBrushDef3Side
{
	public:
		idPlane & Plane(void);
		idVec3 * TextureMatrix(void);
		idStr & Material(void);
		const idPlane & Plane(void) const;
		const idVec3 * TextureMatrix(void) const;
		const idStr & Material(void) const;
		bool PlaneFromPoints(const idVec3 &a, const idVec3 &b, const idVec3 &c);
		const idVec3 & TextureMatrix(int i) const;
		idVec3 & TextureMatrix(int i);
		bool PlaneFromPointAndNormal(const idVec3 &point, const idVec3 &normal);

		friend std::ostream & operator<<(std::ostream &o, const idBrushDef3Side &v);

		bool FromDrawVerts(const idDrawVert verts[3]);

	private:
		idPlane plane;
		idVec3 textureMatrix[2] = {
			{1.0, 0.0, 0.0},
			{0.0, 1.0, 0.0},
		};
		idStr material;
};
using idBrushDef3SideList = std::vector<idBrushDef3Side>;

class idBrushDef3
{
	public:
		idBrushDef3 & operator<<(const idBrushDef3Side &side);
		friend std::ostream & operator<<(std::ostream &o, const idBrushDef3 &v);
		static bool FromBounds(idBrushDef3 &brush, const idBounds &bounds, const idStr &material, float texMat = 1.0f);

	private:
		idBrushDef3SideList sides;
		friend class idEntity;
};
using idBrushDef3List = std::vector<idBrushDef3>;



inline idBrushDef3 & idBrushDef3::operator<<(const idBrushDef3Side &side)
{
	sides.push_back(side);
	return *this;
}

inline idPlane & idBrushDef3Side::Plane(void)
{
	return plane;
}

inline idVec3 * idBrushDef3Side::TextureMatrix(void)
{
	return textureMatrix;
}

inline idStr & idBrushDef3Side::Material(void)
{
	return material;
}

inline const idPlane & idBrushDef3Side::Plane(void) const
{
	return plane;
}

inline const idVec3 * idBrushDef3Side::TextureMatrix(void) const
{
	return textureMatrix;
}

inline const idStr & idBrushDef3Side::Material(void) const
{
	return material;
}

inline const idVec3 & idBrushDef3Side::TextureMatrix(int i) const
{
	return textureMatrix[i];
}

inline idVec3 & idBrushDef3Side::TextureMatrix(int i)
{
	return textureMatrix[i];
}
#endif
