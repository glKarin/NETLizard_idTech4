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
};

#endif