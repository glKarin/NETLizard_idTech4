#ifndef _KARIN_MATERIAL_H
#define _KARIN_MATERIAL_H

#include <iostream>

#include "str.h"

class idMaterial
{
	public:
		idStr name;
		idStr diffusemap;

		friend std::ostream & operator<<(std::ostream &o, const idMaterial &v);

		static const char *AREA_PORTAL_MATERIAL;
		static const char *NODRAW_MATERIAL;
		static const char *CAULK_MATERIAL;
};

#endif
