#include "drawvert.h"

#include "maths.h"
#include "matrix.h"

bool idDrawVert::CalculateFromPoints(idVec3 textureMatrix[2], const idVec3 points[3], const idVec2 uvs[3], const idVec3& normal)
{
	idMat3 transform;
	if(!idMath::CalcTextureMatrixFromPointsXYZUVNormal(transform, points, uvs, normal))
	{
		return false;
	}
    textureMatrix[0][0] = transform(0, 0); // xx
    textureMatrix[0][1] = transform(1, 0); // yx
    textureMatrix[0][2] = transform(2, 0); // zx
    textureMatrix[1][0] = transform(0, 1); // xy
    textureMatrix[1][1] = transform(1, 1); // yy
    textureMatrix[1][2] = transform(2, 1); // zy
	return true;
}