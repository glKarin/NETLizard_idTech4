#include "plane.h"

#include "vector.h"

void idPlane::FromPoints(const idVec3 &a, const idVec3 &b, const idVec3 &c)
{
	idVec3 normal;
	idVec3 first = b - a;
	idVec3 second = c - a;

	normal = first ^ second;
	normal.Normalized();
	normal.FixDegenerateNormal();

	this->d = -(normal * a);
	this->a = normal[0];
	this->b = normal[1];
	this->c = normal[2];
}

void idPlane::FromPointAndNormal(const idVec3 &point, const idVec3 &normal)
{
	this->a = normal[0];
	this->b = normal[1];
	this->c = normal[2];
	this->d = -(normal * point);
}

bool idPlane::IsValid() const
{
	return !Normal().IsZero();
}