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
		idStr & Name();
		idStr & Classname();
		idDict & SpawnArgs();
		const idStr & Name() const;
		const idStr & Classname() const;
		const idDict & SpawnArgs() const;
		idEntity & operator<<(const idBrushDef3 &brush);
		idEntity & operator()(const char *name, const char *value);
		idEntity & operator()(const char *name, float value);
		idEntity & operator()(const char *name, int value);
		idEntity & operator()(const char *name, bool value);
		idEntity & operator()(const char *name, const idVec3 &value);
		idEntity & operator()(const char *name, const idMat3 &value);
		void ClearSpawnArgs();

		friend std::ostream & operator<<(std::ostream &o, const idEntity &v);

	private:
		idStr name;
		idStr classname;
		idBrushDef3List brushs;
		idDict spawnArgs;
};
using idEntityList = std::vector<idEntity>;

inline idEntity & idEntity::operator<<(const idBrushDef3 &brush)
{
	brushs.push_back(brush);
	return *this;
}

inline idStr & idEntity::Name()
{
	return name;
}

inline idStr & idEntity::Classname()
{
	return classname;
}

inline const idStr & idEntity::Name() const
{
	return name;
}

inline const idStr & idEntity::Classname() const
{
	return classname;
}

inline idDict & idEntity::SpawnArgs()
{
	return spawnArgs;
}

inline const idDict & idEntity::SpawnArgs() const
{
	return spawnArgs;
}

inline idEntity & idEntity::operator()(const char *name, const char *value)
{
	spawnArgs.Set(name, value);
	return *this;
}

inline idEntity & idEntity::operator()(const char *name, float value)
{
	spawnArgs.SetFloat(name, value);
	return *this;
}

inline idEntity & idEntity::operator()(const char *name, int value)
{
	spawnArgs.SetInteger(name, value);
	return *this;
}

inline idEntity & idEntity::operator()(const char *name, bool value)
{
	spawnArgs.SetBool(name, value);
	return *this;
}

inline idEntity & idEntity::operator()(const char *name, const idVec3 &value)
{
	spawnArgs.SetVec3(name, value);
	return *this;
}

inline idEntity & idEntity::operator()(const char *name, const idMat3 &value)
{
	spawnArgs.SetMat3(name, value);
	return *this;
}

inline void idEntity::ClearSpawnArgs()
{
	spawnArgs.Clear();
}
#endif
