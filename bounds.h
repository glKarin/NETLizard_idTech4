#ifndef _KARIN_BOUNDS_H
#define _KARIN_BOUNDS_H

#include "vector.h"

class idBounds
{
	public:
		idVec3 b[2] = {idVec3(), idVec3()};

		idVec3 & operator[](int i) {
			return b[i];
		}
		const idVec3 & operator[](int i) const {
			return b[i];
		}
		void AddPoint(const idVec3 &p);
		idVec3 Center(void) const;
		idVec3 Size(void) const;
		idBounds & operator+=(const idBounds &b);
		void Init(const idVec3 &point);
};

inline void idBounds::AddPoint(const idVec3 &p)
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
}

inline idVec3 idBounds::Center(void) const
{
	return (b[0] + b[1]) * 0.5;
}

inline idVec3 idBounds::Size(void) const
{
	return (b[1] - b[0]);
}

inline idBounds & idBounds::operator+=(const idBounds &b)
{
	AddPoint(b[0]);
	AddPoint(b[1]);
	return *this;
}

inline void idBounds::Init(const idVec3 &point)
{
	b[0] = b[1] = point;
}
#endif
