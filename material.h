#ifndef _KARIN_MATERIAL_H
#define _KARIN_MATERIAL_H

#include "str.h"
#include <iostream>

class idMaterial
{
	public:
		idStr name;
		idStr diffusemap;

		friend std::ostream & operator<<(std::ostream &o, const idMaterial &v);
};

#endif
