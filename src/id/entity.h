#ifndef _KARIN_ENTITY_H
#define _KARIN_ENTITY_H

#include <vector>
#include <iostream>

#include "str.h"
#include "dict.h"
#include "brush.h"

class idEntity
{
	public:
		idStr name;
		idStr classname;
		idBrushDef3List brushs;
		idDict spawnArgs;

		friend std::ostream & operator<<(std::ostream &o, const idEntity &v);
};
using idEntityList = std::vector<idEntity>;

#endif
