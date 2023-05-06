#include "brush.h"

#include "matrix.h"
#include "vector.h"
#include "drawvert.h"
#include "maths.h"

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

bool idBrushDef3Side::CalculateFromPoints(idVec3 textureMatrix[2], const idVec3 points[3], const idVec2 uvs[3], const idVec3& normal)
{
	idMat3 transform;
	if(!idMath::CalcTextureMatrixFromPointsXYZUVNormal(transform, points, uvs, normal))
	{
		return false;
	}
    textureMatrix[0][0] = transform(0, 0); // xx
    textureMatrix[0][1] = transform(1, 0); // yx
    textureMatrix[0][2] = transform(2, 0); // zx
    textureMatrix[1][0] = transform(0, 1); // xy
    textureMatrix[1][1] = transform(1, 1); // yy
    textureMatrix[1][2] = transform(2, 1); // zy
	return true;
}

bool idBrushDef3Side::FromDrawVerts(const idDrawVert verts[3])
{
	plane.FromPoints(verts[0].xyz, verts[1].xyz, verts[2].xyz);
	if(plane.Normal().IsZero())
	{
		return false;
	}
	const idVec3 points[] = {verts[0].xyz, verts[1].xyz, verts[2].xyz};
	const idVec2 uvs[] = {verts[0].st, verts[1].st, verts[2].st};
	return CalculateFromPoints(textureMatrix, points, uvs, plane.Normal());
}