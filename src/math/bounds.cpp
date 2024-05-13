#include "bounds.h"

#include "str.h"

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

