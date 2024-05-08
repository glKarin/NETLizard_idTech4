#ifndef _KARIN_MATHS_H
#define _KARIN_MATHS_H

#include <math.h>

#define FLOAT_EPSILON 1e-6

class idVec3;
class idVec2;
class idMat3;

class idMath final
{
    public:
        static bool IsZero(float f);
        static bool CalcTextureMatrixFromPointsXYZUVNormal(idMat3 &mat, const idVec3 points[3], const idVec2 uvs[3], const idVec3& normal);

		template<class T>
			static T Max(const T &a, const T &b);
};


inline bool idMath::IsZero(float f)
{
    return fabs(f) < FLOAT_EPSILON;
}

template<class T>
inline T idMath::Max(const T &a, const T &b)
{
	return a < b ? b : a;
}

#endif
