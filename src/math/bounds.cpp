#include "bounds.h"

#include "str.h"
#include "matrix.h"

idBounds & idBounds::operator+=(const idVec3 &p)
{
	if(p[0] < b[0][0])
		b[0][0] = p[0];
	if(p[0] > b[1][0])
		b[1][0] = p[0];

	if(p[1] < b[0][1])
		b[0][1] = p[1];
	if(p[1] > b[1][1])
		b[1][1] = p[1];

	if(p[2] < b[0][2])
		b[0][2] = p[2];
	if(p[2] > b[1][2])
		b[1][2] = p[2];

	return *this;
}

idStr idBounds::ToString(const char *bc, const char *c) const
{
	idStr str;

	str += b[0].ToString(c);
	str += bc;
	str += b[1].ToString(c);

	return str;
}

void idBounds::ToPoints(idVec3 points[8]) const
{
	for (int i = 0; i < 8; i++) {
		points[i][0] = b[(i^(i>>1))&1][0];
		points[i][1] = b[(i>>1)&1][1];
		points[i][2] = b[(i>>2)&1][2];
	}
}

void idBounds::Expand(float f)
{
	b[0] -= f;
	b[1] += f;
}

void idBounds::Translate(const idVec3 &v)
{
	b[0] += v;
	b[1] += v;
}

void remove_fraction(idBounds &v, int prec)
{
	remove_fraction(v.b[0]);
	remove_fraction(v.b[1]);
}

void idBounds::SetCenter(const idVec3 &v)
{
	idVec3 size = Size() * 0.5f;
	b[0] = v - size;
	b[1] = v + size;
}

void idBounds::FromTransformedBounds(const idBounds &bounds, const idVec3 &origin, const idMat3 &axis)
{
	int i;
	idVec3 center, extents, rotatedExtents;

	center = (bounds[0] + bounds[1]) * 0.5f;
	extents = bounds[1] - center;

	for (i = 0; i < 3; i++) {
		rotatedExtents[i] = idMath::Fabs(extents[0] * axis(0, i)) +
		                    idMath::Fabs(extents[1] * axis(1, i)) +
		                    idMath::Fabs(extents[2] * axis(2, i));
	}

	center = origin + center * axis;
	b[0] = center - rotatedExtents;
	b[1] = center + rotatedExtents;
}

void idBounds::Rotate(const idMat3 &rot)
{
	FromTransformedBounds(*this, {0, 0, 0}, rot);
}

bool idBounds::HasIntersection( const idBounds &a ) const
{
	idVec3 ps[8];
	a.ToPoints(ps);
	for(int i = 0; i < 8; i++)
	{
		if(ContainsPoint(ps[i]))
			return true;
	}
	return false;
}
