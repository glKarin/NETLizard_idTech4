#ifndef _KARIN_BOUNDS_H
#define _KARIN_BOUNDS_H

#include "vector.h"

class idStr;

class idBounds
{
	public:
		idVec3 b[2] = {idVec3(), idVec3()};

		idBounds() = default;
		idBounds(const idVec3 &min, const idVec3 &max);
		idVec3 & operator[](int i);
		const idVec3 & operator[](int i) const;
		idVec3 Center(void) const;
		idVec3 Size(void) const;
		idBounds & operator+=(const idVec3 &p);
		idBounds & operator+=(const idBounds &b);
		void Translate(const idVec3 &v);
		void Init(const idVec3 &point);
		bool IsEmpty(void) const;
		idStr ToString(const char *bc = " ", const char *c = " ") const;
		void ToPoints(idVec3 points[8]) const;
		void Expand(float f);
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

inline bool idBounds::IsEmpty(void) const
{
	return b[0] == b[1];
}

inline idBounds::idBounds(const idVec3 &min, const idVec3 &max)
{
	b[0] = min;
	b[1] = max;
}
#endif
