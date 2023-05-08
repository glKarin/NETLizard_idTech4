#include "brush.h"

#include "vector.h"
#include "drawvert.h"

using std::ostream;

bool idBrushDef3Side::PlaneFromPoints(const idVec3 &a, const idVec3 &b, const idVec3 &c)
{
	plane.FromPoints(a, b, c);
	return plane.IsValid();
}

bool idBrushDef3Side::PlaneFromPointAndNormal(const idVec3 &point, const idVec3 &normal)
{
	plane.FromPointAndNormal(point, normal);
	return plane.IsValid();
}

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

bool idBrushDef3Side::FromDrawVerts(const idDrawVert verts[3])
{
	plane.FromPoints(verts[0].xyz, verts[1].xyz, verts[2].xyz);
	if(!plane.IsValid())
	{
		return false;
	}
	const idVec3 points[] = {verts[0].xyz, verts[1].xyz, verts[2].xyz};
	const idVec2 uvs[] = {verts[0].st, verts[1].st, verts[2].st};
	return idDrawVert::CalculateFromPoints(textureMatrix, points, uvs, plane.Normal());
}