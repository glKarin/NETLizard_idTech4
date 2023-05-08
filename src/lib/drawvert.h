#ifndef _KARIN_VERTEX_H
#define _KARIN_VERTEX_H

#include "vector.h"

class idDrawVert
{
	public:
		idVec3			xyz;
		idVec2			st;
		idVec3			normal;
		//unsigned char			color[4];
		
        static bool CalculateFromPoints(idVec3 mat[2], const idVec3 points[3], const idVec2 uvs[3], const idVec3& normal);
};

#endif