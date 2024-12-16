#include "brush.h"

#include "vector.h"
#include "bounds.h"
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

bool idBrushDef3::FromBounds(idBrushDef3 &brush, const idBounds &bounds,  const idStr &material, float texMat)
{
	if(bounds.IsEmpty())
		return false;

	idBrushDef3Side side;
	side.Material() = material;
	side.TextureMatrix(0)[0] = texMat;
	side.TextureMatrix(1)[1] = texMat;
	float ax = bounds[0][0];
	float ay = bounds[0][1];
	float az = bounds[0][2];
	float bx = bounds[1][0];
	float by = bounds[1][1];
	float bz = bounds[1][2];
	idVec3 front_left_bottom = {ax, ay, az};
	idVec3 back_left_bottom = {ax, by, az};
	idVec3 front_right_bottom = {bx, ay, az};
	idVec3 back_right_bottom = {bx, by, az};
	idVec3 front_left_top = {ax, ay, bz};
	idVec3 back_left_top = {ax, by, bz};
	idVec3 front_right_top = {bx, ay, bz};
	idVec3 back_right_top = {bx, by, bz};

	// front
	if(!side.PlaneFromPoints(front_left_bottom, front_right_bottom, front_left_top))
		return false;
	brush << side;

	// back
	if(!side.PlaneFromPoints(back_right_bottom, back_left_bottom, back_right_top))
		return false;
	brush << side;

	// left
	if(!side.PlaneFromPoints(back_left_bottom, front_left_bottom, back_left_top))
		return false;
	brush << side;

	// right
	if(!side.PlaneFromPoints(front_right_bottom, back_right_bottom, front_right_top))
		return false;
	brush << side;

	// top
	if(!side.PlaneFromPoints(front_left_top, front_right_top, back_left_top))
		return false;
	brush << side;

	// bottom
	if(!side.PlaneFromPoints(back_left_bottom, back_right_bottom, front_left_bottom))
		return false;
	brush << side;
	
	return true;
}
