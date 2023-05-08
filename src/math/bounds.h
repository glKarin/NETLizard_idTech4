#ifndef _KARIN_BOUNDS_H
#define _KARIN_BOUNDS_H

#include "vector.h"

class idBounds
{
	public:
		idVec3 b[2] = {idVec3(), idVec3()};

		idVec3 & operator[](int i);
		const idVec3 & operator[](int i) const;
		idVec3 Center(void) const;
		idVec3 Size(void) const;
		idBounds & operator+=(const idVec3 &p);
		idBounds & operator+=(const idBounds &b);
		void Init(const idVec3 &point);
};

inline idVec3 & idBounds::operator[](int i)
{
	return b[i];
}

inline const idVec3 & idBounds::operator[](int i) const
{
	return b[i];
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
	operator+=(b[0])
	.operator+=(b[1]);
	return *this;
}

inline void idBounds::Init(const idVec3 &point)
{
	b[0] = b[1] = point;
}
#endif
