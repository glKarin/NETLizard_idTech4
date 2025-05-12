#ifndef _KARIN_BOUNDS_H
#define _KARIN_BOUNDS_H

#include <float.h>
#include "vector.h"

class idStr;
class idMat3;

class idBounds
{
	public:
		idVec3 b[2] = {idVec3(FLT_MAX), idVec3(-FLT_MAX)};

		idBounds() = default;
		idBounds(const idVec3 &min, const idVec3 &max);
		idVec3 & operator[](int i);
		const idVec3 & operator[](int i) const;
		idVec3 Center(void) const;
		idVec3 Size(void) const;
		void Zero(void);
		idBounds & operator+=(const idVec3 &p);
		idBounds & operator+=(const idBounds &b);
		void Translate(const idVec3 &v);
		void Rotate(const idMat3 &rot);
		void Init(const idVec3 &point);
		bool IsEmpty(void) const;
		idStr ToString(const char *bc = " ", const char *c = " ") const;
		void ToPoints(idVec3 points[8]) const;
		void Expand(float f);
		void Invalid(void);
		idBounds & operator*=(float scale);
		void SetCenter(const idVec3 &v);
		void FromTransformedBounds(const idBounds &bounds, const idVec3 &origin, const idMat3 &axis);
		bool ContainsPoint(const idVec3 &p) const;
		bool Contains( const idBounds &a ) const;
		bool HasIntersection( const idBounds &a ) const;
};
void remove_fraction(idBounds &v, int prec);

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

inline void idBounds::Invalid(void)
{
	b[0][0] = b[0][1] = b[0][2] = FLT_MAX;
	b[1][0] = b[1][1] = b[1][2] = -FLT_MAX;
}

inline idBounds & idBounds::operator*=(float scale)
{
	b[0] *= scale;
	b[1] *= scale;
	return *this;
}

inline bool idBounds::ContainsPoint(const idVec3 &p) const
{
	if (p[0] < b[0][0] || p[1] < b[0][1] || p[2] < b[0][2]
	    || p[0] > b[1][0] || p[1] > b[1][1] || p[2] > b[1][2]) {
		return false;
	}

	return true;
}

inline void idBounds::Zero(void)
{
	b[0][0] = b[0][1] = b[0][2] =
	                            b[1][0] = b[1][1] = b[1][2] = 0;
}

inline bool idBounds::Contains( const idBounds &a ) const
{
	int		i;

	for( i = 0; i < 3; i++ ) {
		if( a[1][i] > b[1][i] ) {
			return( false );
		}
		if( a[0][i] < b[0][i] ) {
			return( false );
		}
	}

	return( true );
}
#endif
