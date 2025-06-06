#include "maths.h"

#include <iostream>

#include "vector.h"
#include "matrix.h"
#include "str.h"
#include "decimal.h"

bool idMath::CalcTextureMatrixFromPointsXYZUVNormal(idMat3 &mat, const idVec3 points[3], const idVec2 uvs[3], const idVec3& normal)
{
    // Calculate the texture projection for the desired set of UVs and XYZ

    // The texture projection matrix is applied to the vertices after they have been
    // transformed by the axis base transform (which depends on this face's normal):
    // T * AB * vertex = UV
    // 
    // Applying AB to the vertices will yield: T * P = texcoord
    // with P containing the axis-based transformed vertices.
    // 
    // If the above should be solved for T, expanding the above multiplication 
    // sets up six equations to calculate the 6 unknown components of T.
    // 
    // We can arrange the 6 equations in matrix form: T * A = B
    // T is the 3x3 texture matrix.
    // A contains the XY coords in its columns (Z is ignored since we 
    // applied the axis base), B contains the UV coords in its columns.
    // The third component of all columns in both matrices is 1.
    // 
    // We can solve the above by inverting A: T = B * inv(A)

    // Get the axis base for this face, we need the XYZ points in that state
    // to reverse-calculate the desired texture transform
    idMat4 axisBase = normal.GetBasisTransformForNormal();

    // Rotate the three incoming world vertices into the local face plane
    idVec3 localPoints[] =
    { // M * v -> v * M
        points[0] * axisBase,
        points[1] * axisBase,
        points[2] * axisBase,
    };

    // Arrange the XYZ coords into the columns of matrix A
    idMat3 xyz{
		localPoints[0].X(), localPoints[0].Y(), 1,
        localPoints[1].X(), localPoints[1].Y(), 1,
        localPoints[2].X(), localPoints[2].Y(), 1
	};

    idMat3 uv{
		uvs[0].X(), uvs[0].Y(), 1,
        uvs[1].X(), uvs[1].Y(), 1,
        uvs[2].X(), uvs[2].Y(), 1
	};

    // setTransform(uv * xyz);
	xyz.Inverse();
	idMat3 transform = uv * xyz;

    // Check the matrix for validity
	// xx yx xy yy
    if ((transform(0, 0) != 0 || transform(1, 0) != 0) && (transform(0, 1) != 0 || transform(1, 1) != 0))
    {
        mat = transform;
		return true;
    }
    else
    {
		//std::cout << transform.ToString().c_str() << std::endl;
		return false;
    }
}

void remove_fraction(float &f, int prec)
{
	if(prec <= 0)
		f = float(int(f));
	else
	{
		std::string str = idStr::va("%f", f);
#define _DECIMAL_CASE(x) \
			case x: {\
				dec::decimal<x, dec::half_up_round_policy> d##x(str); \
				f = (float)d##x.getAsDouble(); \
		break; \
					}

		switch(prec)
		{
			_DECIMAL_CASE(1);
			_DECIMAL_CASE(2);
			_DECIMAL_CASE(3);
			_DECIMAL_CASE(4);
			_DECIMAL_CASE(5);
			_DECIMAL_CASE(6);
		};
	}
}
#if 0
#define _DECIMAL_CASE(x) \
			case x: {\
				dec::decimal<x, dec::half_up_round_policy> d##x(str); d##x *= pow(10, x); \
						int i = d##x.getAsInteger(); idStr n = idStr::va("%06d", i); n.insert(n.length() - x, ".");  sscanf(n, "%f", &f); printf("%s\n", n.c_str());\
				 \
		break; \
					}
#endif
