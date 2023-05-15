#ifndef _KARIN_ENTITY_H
#define _KARIN_ENTITY_H

#include <iostream>

#include "str.h"
#include "dict.h"
#include "brush.h"
#include "list.h"

class idEntity
{
	public:
		idEntity & Name(const char *str, ...);
		idEntity & NameByClass(const char *str, ...);
		idEntity & Classname(const char *str);
		idDict & SpawnArgs();
		idStr Name() const;
		const idStr & Classname() const;
		const idDict & SpawnArgs() const;
		idEntity & operator<<(const idBrushDef3 &brush);
		idEntity & operator()(const char *name, const char *value);
		idEntity & operator()(const char *name, float value);
		idEntity & operator()(const char *name, int value);
		idEntity & operator()(const char *name, bool value);
		idEntity & operator()(const char *name, const idVec3 &value);
		idEntity & operator()(const char *name, const idMat3 &value);
		void ClearSpawnArgs(void);
		void ClearBrushs(void);
		idEntity & Origin(const idVec3 &v);
		void Reset(void);
		void Model(void);

		idEntity & func_static(void);
		idEntity & func_rotating(bool xAxis = false, bool yAxis = false);
		idEntity & func_bobbing(float height = -1, float speed = -1);
		idEntity & worldspawn(void);
		idEntity & light(const idVec3 &origin, const idVec3 &radius = {0, 0, 0}, bool noshadows = true, bool nospecular = true, const idVec3 &color = {0, 0, 0});
		idEntity & info_player_deathmatch(const idVec3 &origin);
		idEntity & info_player_start(const idVec3 &origin, float angle = 0.0f);
		idEntity & info_player_teleport(const idVec3 &origin, float angle = 0.0f);
		idEntity & target_endLevel(const char *nextMap);
		idEntity & trigger_multiple(const char *target);
		idEntity & func_waitforbutton(const char *target);
		idEntity & func_door(int movedir);

		friend std::ostream & operator<<(std::ostream &o, const idEntity &v);

		static const char *CLASSNAME_FUNC_STATIC;
		static const char *CLASSNAME_FUNC_ROTATING;
		static const char *CLASSNAME_FUNC_BOBBING;
		static const char *CLASSNAME_WORLDSPAWN;
		static const char *CLASSNAME_LIGHT;
		static const char *CLASSNAME_INFO_PLAYER_DEATHMATCH;
		static const char *CLASSNAME_INFO_PLAYER_START;
		static const char *CLASSNAME_INFO_PLAYER_TELEPORT;
		static const char *CLASSNAME_TARGET_ENDLEVEL;
		static const char *CLASSNAME_TRIGGER_MULTIPLE;
		static const char *CLASSNAME_TARGET_WAITFORBUTTON;
		static const char *CLASSNAME_FUNC_DOOR;

	private:
		idStr name;
		idStr classname = "func_static";
		idBrushDef3List brushs;
		idDict spawnArgs;
};
using idEntityList = idList<idEntity>;

inline idEntity & idEntity::operator<<(const idBrushDef3 &brush)
{
	brushs.push_back(brush);
	return *this;
}

inline idEntity & idEntity::Classname(const char *classname)
{
	this->classname = classname;
	return *this;
}

inline idStr idEntity::Name() const
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

inline void idEntity::ClearSpawnArgs(void)
{
	spawnArgs.Clear();
}

inline void idEntity::ClearBrushs(void)
{
	brushs.clear();
}

inline idEntity & idEntity::Origin(const idVec3 &v)
{
	spawnArgs.SetVec3("origin", v);
	return *this;
}

inline void idEntity::Model(void)
{
	spawnArgs.Set("model", Name());
}
#endif
