#include "bounds.h"

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