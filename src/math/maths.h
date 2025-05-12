#ifndef _KARIN_MATHS_H
#define _KARIN_MATHS_H

#include <math.h>
#include <string.h>

#define FLOAT_EPSILON 1e-6

class idVec3;
class idVec2;
class idMat3;

class idMath final
{
    public:
        static bool IsZero(float f);
        static bool CalcTextureMatrixFromPointsXYZUVNormal(idMat3 &mat, const idVec3 points[3], const idVec2 uvs[3], const idVec3& normal);
        static float IntBitsToFloat(int i);
        static float Fabs(float f);

		template<class T>
			static T Max(const T &a, const T &b);
		template<class T>
			static T Min(const T &a, const T &b);
		template<class T>
			static T Clamp(const T &a, const T &b, const T &c);
};

void remove_fraction(float &f, int prec = 0);


inline bool idMath::IsZero(float f)
{
    return fabs(f) < FLOAT_EPSILON;
}

template<class T>
inline T idMath::Max(const T &a, const T &b)
{
	return a < b ? b : a;
}

template<class T>
inline T idMath::Min(const T &a, const T &b)
{
	return a > b ? b : a;
}

template<class T>
inline T idMath::Clamp(const T &i, const T &mi, const T &ma)
{
	return i > ma ? ma : (i < mi ? mi : i);
}

inline float idMath::IntBitsToFloat(int i)
{
    union bit
    {
        int i = 0;
        float f;
    } u;
    memset(&u, 0, sizeof(union bit));
    u.i = i;
    return u.f;
}

inline float idMath::Fabs(float f)
{
	return fabs(f);
}

#endif
