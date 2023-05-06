#ifndef _KARIN_VERTOR_H
#define _KARIN_VERTOR_H

#include <math.h>

class idMat4;
class idStr;

class idVec2
{
	public:
		float v[2] = {0, 0};
		float & operator[](int i) {
			return v[i];
		}
		float operator[](int i) const {
			return v[i];
		}
		operator float *() {
			return v;
		}
		operator const float *() const {
			return v;
		}
		float X() const {
			return v[0];
		}
		float Y() const {
			return v[1];
		}
		float & X() {
			return v[0];
		}
		float & Y() {
			return v[1];
		}
		float & operator()(int i) {
			return v[i];
		}
		float operator()(int i) const {
			return v[i];
		}
};

class idVec3
{
	public:
		float v[3] = {0, 0, 0};

		idVec3() = default;
		idVec3(const float a[3]) {
			Set(a[0], a[1], a[2]);
		}
		idVec3(float x, float y, float z) {
			Set(x, y, z);
		}
		void Set(float x, float y, float z);
		bool IsZero() const;
		void Zero(void);
		float & operator[](int i) {
			return v[i];
		}
		float operator[](int i) const {
			return v[i];
		}
		operator float *() {
			return v;
		}
		operator const float *() const {
			return v;
		}
		float X() const {
			return v[0];
		}
		float Y() const {
			return v[1];
		}
		float Z() const {
			return v[2];
		}
		float & X() {
			return v[0];
		}
		float & Y() {
			return v[1];
		}
		float & Z() {
			return v[2];
		}
		float & operator()(int i) {
			return v[i];
		}
		float operator()(int i) const {
			return v[i];
		}

		friend idVec3 operator+(const idVec3 &a, const idVec3 &b);
		friend idVec3 operator-(const idVec3 &a, const idVec3 &b);
		friend float operator*(const idVec3 &a, const idVec3 &b);
		friend idVec3 operator^(const idVec3 &a, const idVec3 &b);
		friend idVec3 operator/(const idVec3 &v, float f);
		friend idVec3 operator*(const idVec3 &v, float f);
		friend idVec3 operator-(const idVec3 &v);
		idVec3 & operator*=(float n);
		float Length(void) const;
		void Normalized(void);
		bool FixDegenerateNormal(void);
		idStr ToString(const char *c = " ") const;
		void ComputeAxisBase(idVec3 &texS, idVec3 &texT) const;
		idMat4 GetBasisTransformForNormal(void) const;

		static bool IsNear(const idVec3& v1, const idVec3& v2, float epsilon);
		static idVec3 TriangleCaleNormal(const idVec3 &a, const idVec3 &b, const idVec3 &c);
};

inline idVec3 operator+(const idVec3 &a, const idVec3 &b)
{
	float x = a[0] + b[0];
	float y = a[1] + b[1];
	float z = a[2] + b[2];
	return idVec3(x, y, z);
}

inline idVec3 operator-(const idVec3 &a, const idVec3 &b)
{
	float x = a[0] - b[0];
	float y = a[1] - b[1];
	float z = a[2] - b[2];
	return idVec3(x, y, z);
}

inline float operator*(const idVec3 &a, const idVec3 &b)
{
	float x = a[0] * b[0];
	float y = a[1] * b[1];
	float z = a[2] * b[2];
	return x + y + z;
}

inline idVec3 operator^(const idVec3 &a, const idVec3 &b)
{
	float x = a[1] * b[2] - a[2] * b[1];
	float y = a[2] * b[0] - a[0] * b[2];
	float z = a[0] * b[1] - a[1] * b[0];
	return idVec3(x, y, z);
}

inline idVec3 & idVec3::operator*=(float n)
{
	v[0] *= n;
	v[1] *= n;
	v[2] *= n;
	return *this;
}

inline idVec3 operator-(const idVec3 &v)
{
	float x = -v.v[0];
	float y = -v.v[1];
	float z = -v.v[2];
	return idVec3(x, y, z);
}

inline idVec3 operator/(const idVec3 &v, float f)
{
	float x = v.v[0] / f;
	float y = v.v[1] / f;
	float z = v.v[2] / f;
	return idVec3(x, y, z);
}

inline idVec3 operator*(const idVec3 &v, float f)
{
	float x = v.v[0] * f;
	float y = v.v[1] * f;
	float z = v.v[2] * f;
	return idVec3(x, y, z);
}

inline float idVec3::Length(void) const
{
#define POW2(n) ((n) * (n))
	return sqrt(POW2(v[0]) + POW2(v[1]) + POW2(v[2]));
#undef POW2
}

inline void idVec3::Normalized(void)
{
	float m;

	m = Length();
	if(m != 0.0)
	{
		m = 1.0 / m;
		v[0] *= m;
		v[1] *= m;
		v[2] *= m;
	}
}

inline bool idVec3::IsZero() const
{
	return v[0] == 0.0 && v[1] == 0.0 && v[2] == 0.0;
}

inline void idVec3::Set(float x, float y, float z)
{
	v[0] = x;
	v[1] = y;
	v[2] = z;
}

inline void idVec3::Zero(void)
{
	v[0] = v[1] = v[2] = 0;
}
#endif
