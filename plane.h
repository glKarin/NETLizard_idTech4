#ifndef _KARIN_PLANE_H
#define _KARIN_PLANE_H

#include "vector.h"

class idPlane
{
	public:
		float a = 0.0;
		float b = 0.0;
		float c = 1.0;
		float d = 0.0;

		idPlane() = default;
		idPlane(float a, float b, float c, float d)
			: a(a),
			b(b),
			c(c),
			d(d)
	{}

		idPlane(const idVec3 &a, const idVec3 &b, const idVec3 &c) {
			FromPoints(a, b, c);
		}
		
		operator const float *() {
			return reinterpret_cast<const float *>(&a);
		}
		float & operator[](int i) {
			return reinterpret_cast<float *>(&a)[i];
		}
		float operator[](int i) const {
			return reinterpret_cast<const float *>(&a)[i];
		}
		const idVec3 & Normal() const {
			return *reinterpret_cast<const idVec3 *>(&a);
		}
		void FromPoints(const idVec3 &a, const idVec3 &b, const idVec3 &c);
		void FromPointAndNormal(const idVec3 &point, const idVec3 &normal);
};

inline void idPlane::FromPoints(const idVec3 &a, const idVec3 &b, const idVec3 &c)
{
	idVec3 normal;
	idVec3 first = b - a;
	idVec3 second = c - a;

	normal = first ^ second;
	normal.Normalize();
	normal.FixDegenerateNormal();

	this->d = -(normal * a);
	this->a = normal[0];
	this->b = normal[1];
	this->c = normal[2];
}

inline void idPlane::FromPointAndNormal(const idVec3 &point, const idVec3 &normal)
{
	this->a = normal[0];
	this->b = normal[1];
	this->c = normal[2];
	this->d = -(normal * point);
}
#endif
