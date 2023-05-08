#ifndef _KARIN_PLANE_H
#define _KARIN_PLANE_H

class idVec3;

class idPlane
{
	public:
		float a = 0.0;
		float b = 0.0;
		float c = 1.0;
		float d = 0.0;

		idPlane() = default;
		idPlane(float a, float b, float c, float d);
		idPlane(const idVec3 &a, const idVec3 &b, const idVec3 &c);
		
		operator const float *();
		float & operator[](int i);
		float operator[](int i) const;
		const idVec3 & Normal() const;
		idVec3 & Normal();
		void FromPoints(const idVec3 &a, const idVec3 &b, const idVec3 &c);
		void FromPointAndNormal(const idVec3 &point, const idVec3 &normal);
		bool IsValid() const;
};

inline idPlane::idPlane(float a, float b, float c, float d)
	: a(a),
	b(b),
	c(c),
	d(d)
{
}

inline idPlane::idPlane(const idVec3 &a, const idVec3 &b, const idVec3 &c)
{
	FromPoints(a, b, c);
}

inline idPlane::operator const float *()
{
	return reinterpret_cast<const float *>(&a);
}

inline float & idPlane::operator[](int i)
{
	return reinterpret_cast<float *>(&a)[i];
}

inline float idPlane::operator[](int i) const
{
	return reinterpret_cast<const float *>(&a)[i];
}

inline const idVec3 & idPlane::Normal() const
{
	return *reinterpret_cast<const idVec3 *>(&a);
}

inline idVec3 & idPlane::Normal()
{
	return *reinterpret_cast<idVec3 *>(&a);
}
#endif
