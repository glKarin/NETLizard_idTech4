#include "vector.h"

#include "matrix.h"
#include "str.h"
#include "maths.h"

idStr idVec2::ToString(const char *c) const
{
	return idStr::va("%f%s%f", v[0], c, v[1]);
}



bool idVec3::IsNear(const idVec3& v1, const idVec3& v2, float epsilon)
{
    idVec3 diff = v1 - v2;
    return fabs(diff[0]) < epsilon && fabs(diff[1]) < epsilon
        && fabs(diff[2]) < epsilon;
}

idStr idVec3::ToString(const char *c) const
{
	return idStr::va("%f%s%f%s%f", v[0], c, v[1], c, v[2]);
}

idVec3 idVec3::TriangleCaleNormal(const idVec3 &a, const idVec3 &b, const idVec3 &c)
{
	idVec3 normal;
	idVec3 first = b - a;
	idVec3 second = c - a;

	//vector3_crossv(normal, &second, &first); // org
	normal = first ^ second;
	normal.Normalized();
	return normal;
}

bool idVec3::FixDegenerateNormal(void)
{
	if (v[0] == 0.0f) {
		if (v[1] == 0.0f) {
			if (v[2] > 0.0f) {
				if (v[2] != 1.0f) {
					v[2] = 1.0f;
					return true;
				}
			} else {
				if (v[2] != -1.0f) {
					v[2] = -1.0f;
					return true;
				}
			}

			return false;
		} else if (v[2] == 0.0f) {
			if (v[1] > 0.0f) {
				if (v[1] != 1.0f) {
					v[1] = 1.0f;
					return true;
				}
			} else {
				if (v[1] != -1.0f) {
					v[1] = -1.0f;
					return true;
				}
			}

			return false;
		}
	} else if (v[1] == 0.0f) {
		if (v[2] == 0.0f) {
			if (v[0] > 0.0f) {
				if (v[0] != 1.0f) {
					v[0] = 1.0f;
					return true;
				}
			} else {
				if (v[0] != -1.0f) {
					v[0] = -1.0f;
					return true;
				}
			}

			return false;
		}
	}

	if (fabs(v[0]) == 1.0f) {
		if (v[1] != 0.0f || v[2] != 0.0f) {
			v[1] = v[2] = 0.0f;
			return true;
		}

		return false;
	} else if (fabs(v[1]) == 1.0f) {
		if (v[0] != 0.0f || v[2] != 0.0f) {
			v[0] = v[2] = 0.0f;
			return true;
		}

		return false;
	} else if (fabs(v[2]) == 1.0f) {
		if (v[0] != 0.0f || v[1] != 0.0f) {
			v[0] = v[1] = 0.0f;
			return true;
		}

		return false;
	}

	return false;
}

void idVec3::ComputeAxisBase(idVec3 &texS, idVec3 &texT) const
{
#if 1 // from DarkRadiant
	static const idVec3 up(0, 0, 1);
	static const idVec3 down(0, 0, -1);
	const idVec3 &normal = *this;

	if (IsNear(normal, up, 1e-6)) // straight up?
	{
		texS.Set(0, 1, 0);
		texT.Set(1, 0, 0);
	}
	else if (IsNear(normal, down, 1e-6)) // straight down?
	{
		texS.Set(0, 1, 0);
		texT.Set(-1, 0, 0);
	}
	else
	{
		texS = normal ^ up;
		texT = normal ^ texS;
		texS.Normalized();
		texT.Normalized();
		texS = -texS;
	}
#else // from DOOM3
	float RotY, RotZ;
	idVec3 n;

	// do some cleaning
	n[0] = idMath::IsZero(v[0]) ? 0.0f : v[0];
	n[1] = idMath::IsZero(v[1]) ? 0.0f : v[1];
	n[2] = idMath::IsZero(v[2]) ? 0.0f : v[2];

	RotY = -atan2(n[2], sqrt(n[1] * n[1] + n[0] * n[0]));
	RotZ = atan2(n[1], n[0]);
	// rotate (0,1,0) and (0,0,1) to compute texS and texT
	texS[0] = -sin(RotZ);
	texS[1] = cos(RotZ);
	texS[2] = 0;
	// the texT vector is along -Z ( T texture coorinates axis )
	texT[0] = -sin(RotY) * cos(RotZ);
	texT[1] = -sin(RotY) * sin(RotZ);
	texT[2] = -cos(RotY);
#endif
}

idMat4 idVec3::GetBasisTransformForNormal(void) const
{
	idVec3 normal = *this;
    idVec3 texS, texT;
    ComputeAxisBase(texS, texT);

    idMat4 basis;
    basis.SetCol3(0, texS);
    basis.SetCol3(1, texT);
    basis.SetCol3(2, normal);

    // At this point the basis matrix contains three column vectors that are
    // perpendicular to each other.

    // The x-line of <basis> contains the <texS> basis vector (within the face plane)
    // The y-line of <basis> contains the <texT> basis vector (within the face plane)
    // The z-line of <basis> contains the <normal> basis vector (perpendicular to the face plane)

    // invert this matrix and return
	basis.Transposed();
    return basis; 
}

float idVec3::GetMaxAxisLength(void) const
{
	float x = fabs(v[0]);
	float y = fabs(v[1]);
	float z = fabs(v[2]);
	return idMath::Max(x, idMath::Max(y, z));
}

idVec3 idVec3::MadSat(const idVec3 &a, const idVec3 &b, float len)
{
	idVec3 v = a - b;
	v.Normalized();
	return a + v * len;
}
