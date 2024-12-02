#include "entity.h"

#include <stdarg.h>
#include <stdio.h>

#include "math/bounds.h"

const char * idEntity::CLASSNAME_FUNC_STATIC = "func_static";
const char * idEntity::CLASSNAME_FUNC_ROTATING = "func_rotating";
const char * idEntity::CLASSNAME_FUNC_BOBBING = "func_bobbing";
const char * idEntity::CLASSNAME_WORLDSPAWN = "worldspawn";
const char * idEntity::CLASSNAME_LIGHT = "light";
const char * idEntity::CLASSNAME_INFO_PLAYER_DEATHMATCH = "info_player_deathmatch";
const char * idEntity::CLASSNAME_INFO_PLAYER_START = "info_player_start";
const char * idEntity::CLASSNAME_INFO_PLAYER_TELEPORT = "info_player_teleport";
const char * idEntity::CLASSNAME_TARGET_ENDLEVEL = "target_endLevel";
const char * idEntity::CLASSNAME_TRIGGER_MULTIPLE = "trigger_multiple";
const char * idEntity::CLASSNAME_TARGET_WAITFORBUTTON = "func_waitforbutton";
const char * idEntity::CLASSNAME_FUNC_DOOR = "func_door";
const char * idEntity::CLASSNAME_FUNC_ELEVATOR = "func_elevator";
const char * idEntity::CLASSNAME_TEXT = "text";

idEntity & idEntity::Name(const char *str, ...)
{
	va_list va;

	va_start(va, str);
	name = idStr::va(str, va);
	va_end(va);

	return *this;
}

idEntity & idEntity::NameByClass(const char *str, ...)
{
	va_list va;

	va_start(va, str);
	name = classname + idStr::va(str, va);
	va_end(va);

	return *this;
}

std::ostream & operator<<(std::ostream &o, const idEntity &v)
{
	o << "{\n";
	o << idStr::va("\"classname\" \"%s\"\n", v.classname.c_str());
	o << idStr::va("\"name\" \"%s\"\n", v.name.c_str());
	if(!v.spawnArgs.IsEmpty())
		o << v.spawnArgs;

	if(!v.brushs.empty())
	{
		int i = 0;
		for(const auto &v : v.brushs)
		{
			o << idStr::va("//primitive %d\n", i++);
			o << "{\n";
			o << v;
			o << "}\n";
		}
	}
	o << "}\n";
	return o;
}

void idEntity::Reset(void)
{
	Name("");
	classname = CLASSNAME_FUNC_STATIC;
	ClearSpawnArgs();
	ClearBrushs();
}

idEntity & idEntity::worldspawn(void)
{
	Reset();
	classname = CLASSNAME_WORLDSPAWN;
	name = "world";
	return *this;
}

idEntity & idEntity::func_static(void)
{
	Reset();
	classname = CLASSNAME_FUNC_STATIC;
	return *this;
}

idEntity & idEntity::func_rotating(bool xAxis, bool yAxis)
{
	Reset();
	classname = CLASSNAME_FUNC_ROTATING;
	spawnArgs.SetBool("x_axis", xAxis);
	spawnArgs.SetBool("y_axis", yAxis);
	return *this;
}

idEntity & idEntity::func_bobbing(float height, float speed)
{
	Reset();
	classname = CLASSNAME_FUNC_BOBBING;
	if(height > 0)
		spawnArgs.SetFloat("height", height);
	if(speed > 0)
		spawnArgs.SetFloat("speed", speed);
	return *this;
}

idEntity & idEntity::info_player_deathmatch(const idVec3 &origin)
{
	Reset();
	classname = CLASSNAME_INFO_PLAYER_DEATHMATCH;
	Origin(origin);
	return *this;
}

idEntity & idEntity::info_player_start(const idVec3 &origin, float angle)
{
	Reset();
	classname = CLASSNAME_INFO_PLAYER_START;
	Origin(origin);
	spawnArgs.SetFloat("angle", angle);
	return *this;
}

idEntity & idEntity::info_player_teleport(const idVec3 &origin, float angle)
{
	Reset();
	classname = CLASSNAME_INFO_PLAYER_TELEPORT;
	Origin(origin);
	spawnArgs.SetFloat("angle", angle);
	return *this;
}

idEntity & idEntity::light(const idVec3 &origin, const idVec3 &radius, bool noshadows, bool nospecular, const idVec3 &color)
{
	Reset();
	classname = CLASSNAME_LIGHT;
	Origin(origin);
	if(!radius.IsZero())
		spawnArgs.SetVec3("light_radius", radius);
	spawnArgs.SetBool("noshadows", noshadows);
	spawnArgs.SetBool("nospecular", nospecular);
	if(!color.IsZero())
		spawnArgs.SetVec3("_color", color);

	spawnArgs.SetInteger("falloff", 0);
	return *this;
}

idEntity & idEntity::target_endLevel(const char *nextMap)
{
	Reset();
	classname = CLASSNAME_TARGET_ENDLEVEL;
	spawnArgs.Set("nextMap", nextMap);
	spawnArgs.SetBool("noflood", true);
	return *this;
}

idEntity & idEntity::trigger_multiple(const char *target)
{
	Reset();
	classname = CLASSNAME_TRIGGER_MULTIPLE;
	spawnArgs.Set("target", target);
	return *this;
}

idEntity & idEntity::func_waitforbutton(const char *target)
{
	Reset();
	classname = CLASSNAME_TARGET_WAITFORBUTTON;
	spawnArgs.Set("target", target);
	return *this;
}

idEntity & idEntity::func_door(int movedir)
{
	Reset();
	classname = CLASSNAME_FUNC_DOOR;
	spawnArgs.SetInteger("movedir", movedir);
	return *this;
}

void idEntity::MinMax(const idBounds &bounds)
{
	spawnArgs.SetVec3("mins", bounds[0]);
	spawnArgs.SetVec3("maxs", bounds[1]);
}

void idEntity::Size(const idVec3 &size)
{
	spawnArgs.SetVec3("size", size);
}

idEntity & idEntity::func_elevator(float moveSpeed, float moveTime, bool trigger, int numFloor, const idVec3 floors[])
{
	Reset();
	classname = CLASSNAME_FUNC_ELEVATOR;
	spawnArgs.SetFloat("move_speed", moveSpeed);
	spawnArgs.SetFloat("move_time", moveTime);
	spawnArgs.SetBool("trigger", trigger);
	spawnArgs.SetBool("solid", true);
	for(int i = 0; i < numFloor; i++)
	{
		spawnArgs.SetVec3(idStr::va("floorPos_%d", i + 1), floors[i]);
	}
	if(/*trigger && */numFloor > 0)
	{
		spawnArgs.SetInteger("triggerFloor", numFloor);
		spawnArgs.SetInteger("returnFloor", 1);
	}
	return *this;
}

idEntity & idEntity::text(const char *str, bool face)
{
	Reset();
	classname = CLASSNAME_TEXT;
	spawnArgs.Set("text", str);
	spawnArgs.SetBool("force", true);
	spawnArgs.SetBool("playerOriented", face);
	return *this;
}
